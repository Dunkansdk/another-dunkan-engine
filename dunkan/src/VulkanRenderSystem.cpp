#include "vulkan/VulkanRenderSystem.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "game/types.hpp"

struct UniformBufferObject {
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

// Type aliases for foreach template
using VulkanRenderSystem_c = ADE::META_TYPES::Typelist<RenderComponent, PhysicsComponent>;
using VulkanRenderSystem_t = ADE::META_TYPES::Typelist<>;

VulkanRenderSystem::VulkanRenderSystem(VulkanContext& context, VulkanDescriptorManager& descriptorManager,
                                       VulkanPipeline& pipeline, EntityManager& entityManager)
    : m_context(context), m_descriptorManager(descriptorManager),
      m_pipeline(pipeline), m_entityManager(entityManager) {
    
    createDefaultTexture();
    
    // Create persistent quad vertex buffer
    std::vector<Vertex> quadVertices;
    createQuadVertices(quadVertices, glm::vec2(1.0f, 1.0f)); // Unit quad, will scale with push constants
    
    m_quadVertexBuffer = new VulkanBuffer(m_context);
    
    VulkanBuffer stagingBuffer(m_context);
    VkDeviceSize bufferSize = quadVertices.size() * sizeof(Vertex);
    stagingBuffer.create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stagingBuffer.copyFrom(quadVertices.data(), bufferSize);
    
    m_quadVertexBuffer->create(bufferSize,
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    stagingBuffer.copyTo(*m_quadVertexBuffer, bufferSize);
    stagingBuffer.cleanup();
    
    // Create uniform buffer
    bufferSize = sizeof(UniformBufferObject);
    m_uniformBuffer = new VulkanBuffer(m_context);
    m_uniformBuffer->create(bufferSize,
                           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    
    // Create lighting uniform buffer
    m_lightingUBO = new VulkanBuffer(m_context);
    m_lightingUBO->create(sizeof(void*) * 100,  // Placeholder size, will be set from main
                         VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    
    // Create descriptor sets for each frame
    m_descriptorSets.resize(MAX_FRAMES);
    for (int i = 0; i < MAX_FRAMES; i++) {
        m_descriptorSets[i] = m_descriptorManager.allocateDescriptorSet(m_pipeline.getDescriptorSetLayout());
        
        m_descriptorManager.updateUniformBuffer(m_descriptorSets[i], 0,
                                               m_uniformBuffer->getBuffer(), sizeof(UniformBufferObject));
        
        m_descriptorManager.updateTextureDescriptor(m_descriptorSets[i], 1,
                                                    m_defaultTexture->getImageView(),
                                                    m_defaultTexture->getSampler());
        m_descriptorManager.updateTextureDescriptor(m_descriptorSets[i], 2,
                                                    m_defaultTexture->getImageView(),
                                                    m_defaultTexture->getSampler());
        m_descriptorManager.updateTextureDescriptor(m_descriptorSets[i], 3,
                                                    m_defaultTexture->getImageView(),
                                                    m_defaultTexture->getSampler());
        m_descriptorManager.updateTextureDescriptor(m_descriptorSets[i], 4,
                                                    m_defaultTexture->getImageView(),
                                                    m_defaultTexture->getSampler());
    }
}

VulkanRenderSystem::~VulkanRenderSystem() {
    delete m_quadVertexBuffer;
    delete m_uniformBuffer;
    delete m_defaultTexture;
    m_gbuffer.cleanup(m_context);
}

void VulkanRenderSystem::initGBuffer(VkRenderPass renderPass, VkExtent2D extent) {
    m_gbuffer.renderPass = renderPass;
    m_gbuffer.create(m_context, extent.width, extent.height);
    
    // Create G-Buffer framebuffer
    std::array<VkImageView, 5> attachments = {
        m_gbuffer.colorRT->getImageView(),
        m_gbuffer.normalRT->getImageView(),
        m_gbuffer.depthRT->getImageView(),
        m_gbuffer.materialRT->getImageView(),
        m_gbuffer.depthStencilImage->getImageView()
    };
    
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;
    
    if (vkCreateFramebuffer(m_context.getDevice(), &framebufferInfo, nullptr, &m_gbuffer.framebuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create G-Buffer framebuffer!");
    }
}

void VulkanRenderSystem::createDefaultTexture() {
    m_defaultTexture = new VulkanImage(m_context);
    
    // Create 64x64 white texture with subtle pattern
    const int SIZE = 64;
    std::vector<uint32_t> pixels(SIZE * SIZE);
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            // Checkerboard pattern
            bool checker = ((x / 8) + (y / 8)) % 2;
            pixels[y * SIZE + x] = checker ? 0xFFFFFFFF : 0xFFEEEEEE;
        }
    }
    
    m_defaultTexture->createImage(SIZE, SIZE, VK_FORMAT_R8G8B8A8_SRGB,
                                  VK_IMAGE_TILING_OPTIMAL,
                                  VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    
    VulkanBuffer stagingBuffer(m_context);
    stagingBuffer.create(pixels.size() * sizeof(uint32_t), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stagingBuffer.copyFrom(pixels.data(), pixels.size() * sizeof(uint32_t));
    
    m_defaultTexture->transitionLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    m_defaultTexture->copyFromBuffer(stagingBuffer.getBuffer(), SIZE, SIZE);
    m_defaultTexture->transitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    
    stagingBuffer.cleanup();
    
    m_defaultTexture->createImageView(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
    m_defaultTexture->createSampler();
}

void VulkanRenderSystem::createQuadVertices(std::vector<Vertex>& vertices, glm::vec2 size) {
    // Create vertices with WHITE color so textures show properly
    // Y texture coordinates flipped (0.0 at top, 1.0 at bottom) to match Vulkan convention
    vertices = {
        {{0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},        // Top-left (Y=1.0)
        {{size.x, 0.0f}, {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},      // Top-right (Y=1.0)
        {{size.x, size.y}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},    // Bottom-right (Y=0.0)
        
        {{0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},        // Top-left (Y=1.0)
        {{size.x, size.y}, {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},    // Bottom-right (Y=0.0)
        {{0.0f, size.y}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}       // Bottom-left (Y=0.0)
    };
}

void VulkanRenderSystem::prepareFrame(VkCommandBuffer commandBuffer, uint32_t frameIndex) {
    // Update uniform buffer
    UniformBufferObject ubo{};
    ubo.view = glm::mat4(1.0f);
    ubo.proj = glm::ortho(0.0f, 1920.0f, 1080.0f, 0.0f, -100.0f, 100.0f);
    
    m_uniformBuffer->copyFrom(&ubo, sizeof(ubo));
    
    // Bind pipeline (descriptor sets are bound per-entity in renderEntities)
    // Note: We don't bind pipeline here anymore because renderEntities begins the render pass
    // and binding pipeline must happen inside a render pass.
}

void VulkanRenderSystem::renderEntities(VkCommandBuffer commandBuffer, uint32_t frameIndex) {
    // Begin G-Buffer Render Pass
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_gbuffer.renderPass;
    renderPassInfo.framebuffer = m_gbuffer.framebuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = {m_gbuffer.width, m_gbuffer.height};
    
    std::array<VkClearValue, 5> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}}; // Color
    clearValues[1].color = {{0.0f, 0.0f, 0.0f, 1.0f}}; // Normal
    clearValues[2].color = {{0.0f, 0.0f, 0.0f, 1.0f}}; // Depth
    clearValues[3].color = {{0.0f, 0.0f, 0.0f, 1.0f}}; // Material
    clearValues[4].depthStencil = {1.0f, 0};           // Depth/Stencil
    
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();
    
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.getPipeline());
    
    // Bind the persistent quad vertex buffer once
    VkBuffer vertexBuffers[] = {m_quadVertexBuffer->getBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    
    int entitiesRendered = 0;
    
    // Use foreach to iterate entities with RenderComponent and PhysicsComponent
    m_entityManager.foreach<VulkanRenderSystem_c, VulkanRenderSystem_t>
    ([&](Entity&, RenderComponent& renderComp, PhysicsComponent& physicsComp)
    {
        glm::vec2 position(physicsComp.x, physicsComp.y);
        
        // Get size from texture rect
        glm::vec2 size = glm::vec2(renderComp.textureRect.z, renderComp.textureRect.w);
        if (size.x <= 0) size.x = 100.0f;
        if (size.y <= 0) size.y = 100.0f;
        
        // Get descriptor set for this entity's texture
        VkDescriptorSet descriptorSet = m_descriptorSets[frameIndex]; // Default
        if (!renderComp.albedoTextureName.empty()) {
            auto it = m_textureDescriptorSets.find(renderComp.albedoTextureName);
            if (it != m_textureDescriptorSets.end()) {
                descriptorSet = it->second;
            }
        }
        
        // Bind the correct descriptor set for this entity
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                               m_pipeline.getLayout(), 0, 1, &descriptorSet, 0, nullptr);
        
        // Push constants for transform
        struct PushConstants {
            glm::mat4 model;
            float z_position;
            float height;
            int useDepthMap;
        } pushConstants;
        
        // Apply scale and translate
        pushConstants.model = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f)) *
                             glm::scale(glm::mat4(1.0f), glm::vec3(size.x * renderComp.scale, 
                                                                    size.y * renderComp.scale, 1.0f));
        pushConstants.z_position = physicsComp.z;
        pushConstants.height = renderComp.height;
        pushConstants.useDepthMap = (!renderComp.depthTextureName.empty()) ? 1 : 0;
        
        vkCmdPushConstants(commandBuffer, m_pipeline.getLayout(),
                          VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                          0, sizeof(pushConstants), &pushConstants);
        
        vkCmdDraw(commandBuffer, 6, 1, 0, 0);
        entitiesRendered++;
    });
    
    vkCmdEndRenderPass(commandBuffer);
    
    // FPS counter only printed once per second in main loop
}

void VulkanRenderSystem::loadTexture(const std::string& name, const std::string& filepath, 
                                     const std::string& depthFilepath,
                                     const std::string& normalFilepath,
                                     const std::string& materialFilepath) {
    std::cout << "VulkanRenderSystem::loadTexture called for " << name << std::endl;
    // Check if already loaded
    if (m_textures.find(name) != m_textures.end()) {
        std::cout << "Texture '" << name << "' already loaded, skipping." << std::endl;
        return;
    }
    
    std::cout << "Creating VulkanImage..." << std::endl;
    try {
        // Load albedo texture
        VulkanImage* texture = new VulkanImage(m_context);
        std::cout << "Loading albedo from file: " << filepath << std::endl;
        texture->loadFromFile(filepath);
        std::cout << "Creating image view..." << std::endl;
        texture->createImageView(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
        std::cout << "Creating sampler..." << std::endl;
        texture->createSampler();
        
        m_textures[name] = texture;
        
        // Load depth texture if provided
        VulkanImage* depthTexture = m_defaultTexture;
        if (!depthFilepath.empty()) {
            VulkanImage* dTex = new VulkanImage(m_context);
            try {
                dTex->loadFromFile(depthFilepath);
                dTex->createImageView(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
                dTex->createSampler();
                depthTexture = dTex;
                m_textures[name + "_depth_internal"] = dTex;
            } catch (...) {
                std::cerr << "Failed to load depth texture: " << depthFilepath << ", using default." << std::endl;
                delete dTex;
            }
        }
        
        // Load normal texture if provided
        VulkanImage* normalTexture = m_defaultTexture;
        if (!normalFilepath.empty()) {
            VulkanImage* nTex = new VulkanImage(m_context);
            try {
                std::cout << "Loading normal from file: " << normalFilepath << std::endl;
                nTex->loadFromFile(normalFilepath);
                nTex->createImageView(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
                nTex->createSampler();
                normalTexture = nTex;
                m_textures[name + "_normal_internal"] = nTex;
            } catch (...) {
                std::cerr << "Failed to load normal texture: " << normalFilepath << ", using default." << std::endl;
                delete nTex;
            }
        }
        
        // Load material texture if provided
        VulkanImage* materialTexture = m_defaultTexture;
        if (!materialFilepath.empty()) {
            VulkanImage* mTex = new VulkanImage(m_context);
            try {
                std::cout << "Loading material from file: " << materialFilepath << std::endl;
                mTex->loadFromFile(materialFilepath);
                mTex->createImageView(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
                mTex->createSampler();
                materialTexture = mTex;
                m_textures[name + "_material_internal"] = mTex;
            } catch (...) {
                std::cerr << "Failed to load material texture: " << materialFilepath << ", using default." << std::endl;
                delete mTex;
            }
        }
        
        // Create descriptor set for this texture
        std::cout << "Allocating descriptor set for " << name << std::endl;
        VkDescriptorSet descriptorSet = m_descriptorManager.allocateDescriptorSet(m_pipeline.getDescriptorSetLayout());
        
        // Update descriptor set with UBO and all textures
        std::cout << "Updating descriptor set for " << name << std::endl;
        m_descriptorManager.updateUniformBuffer(descriptorSet, 0,
                                               m_uniformBuffer->getBuffer(), sizeof(UniformBufferObject));
        
        // Binding 1: Albedo texture
        m_descriptorManager.updateTextureDescriptor(descriptorSet, 1,
                                                    texture->getImageView(),
                                                    texture->getSampler());
        // Binding 2: Depth texture
        std::cout << "Binding depth texture for " << name << std::endl;
        m_descriptorManager.updateTextureDescriptor(descriptorSet, 2,
                                                    depthTexture->getImageView(),
                                                    depthTexture->getSampler());
        
        // Binding 3: Normal texture
        std::cout << "Binding normal texture for " << name << std::endl;
        m_descriptorManager.updateTextureDescriptor(descriptorSet, 3,
                                                    normalTexture->getImageView(),
                                                    normalTexture->getSampler());
        
        // Binding 4: Material texture
        std::cout << "Binding material texture for " << name << std::endl;
        m_descriptorManager.updateTextureDescriptor(descriptorSet, 4,
                                                    materialTexture->getImageView(),
                                                    materialTexture->getSampler());
        
        m_textureDescriptorSets[name] = descriptorSet;
        
        std::cout << "Loaded texture: " << name << " from " << filepath << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to load texture '" << name << "' from '" << filepath << "': " << e.what() << std::endl;
        // Use default texture as fallback
        m_textures[name] = m_defaultTexture;
        m_textureDescriptorSets[name] = m_descriptorSets[0]; // Use default descriptor set
    }
}

VulkanImage* VulkanRenderSystem::getTexture(const std::string& name) {
    auto it = m_textures.find(name);
    if (it != m_textures.end()) {
        return it->second;
    }
    return m_defaultTexture;  // Return default if not found
}
