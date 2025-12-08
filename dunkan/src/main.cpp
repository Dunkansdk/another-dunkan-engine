#include <GLFW/glfw3.h>
#include <array>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan.h>

#include "ecs/entitymanager.hpp"
#include "game/components/lightcomponent.hpp"
#include "game/components/physicscomponent.hpp"
#include "game/components/rendercomponent.hpp"
#include "game/lightingsystem.hpp"
#include "game/types.hpp"
#include "vulkan/VulkanBuffer.hpp"
#include "vulkan/VulkanContext.hpp"
#include "vulkan/VulkanDescriptorManager.hpp"
#include "vulkan/VulkanImage.hpp"
#include "vulkan/VulkanPipeline.hpp"
#include "vulkan/VulkanRenderPass.hpp"
#include "vulkan/VulkanRenderSystem.hpp"
#include "vulkan/VulkanResourceManager.hpp"
#include "vulkan/VulkanSSAO.hpp"
#include "vulkan/VulkanSwapchain.hpp"
#include "vulkan/VulkanTypes.hpp"

// Application components
#include "app/ApplicationConfig.hpp"
#include "app/DebugUI.hpp"
#include "app/LightingManager.hpp"

// Type aliases for entity iteration
using VulkanRenderSystem_c =
    ADE::META_TYPES::Typelist<RenderComponent, PhysicsComponent>;
using VulkanRenderSystem_t = ADE::META_TYPES::Typelist<>;

const int WIDTH = 1920;
const int HEIGHT = 1080;
const int MAX_FRAMES_IN_FLIGHT = 2;

unsigned int m_frame = 0;
unsigned int m_fps = 0;

struct UniformBufferObject {
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 proj;
};

class VulkanApplication {
public:
  void run() {
    initWindow();
    initVulkan();
    loadGameEntities();
    mainLoop();
    cleanup();
  }

private:
  GLFWwindow *window = nullptr;
  VulkanContext *vulkanContext = nullptr;
  VulkanSwapchain *swapchain = nullptr;
  VulkanRenderPass *renderPass = nullptr;
  VulkanPipeline *pipeline = nullptr;
  VulkanPipeline *compPipeline = nullptr;
  VulkanDescriptorManager *descriptorManager = nullptr;
  VulkanRenderSystem *renderSystem = nullptr;
  VulkanSSAO *ssao = nullptr;
  EntityManager entity_manager;

  std::vector<VkCommandBuffer> commandBuffers;
  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;
  std::vector<VkDescriptorSet> descriptorSets;
  VkDescriptorSet compDescriptorSet;
  uint32_t currentFrame = 0;

  // ImGui resources
  VkDescriptorPool imguiDescriptorPool = VK_NULL_HANDLE;

  // Application components (modular design)
  dunkan::ApplicationConfig config;
  dunkan::LightingManager lightingManager;
  std::unique_ptr<dunkan::DebugUI> debugUI;

  // Entity editing cache (for DebugUI)
  std::vector<dunkan::EntityEditData> entityEditCache;
  bool entityCacheNeedsRebuild = true;

  void initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Dunkan Engine - Vulkan", nullptr,
                              nullptr);
  }

  void initVulkan() {
    vulkanContext = new VulkanContext();
    vulkanContext->init(window);

    swapchain = new VulkanSwapchain(*vulkanContext, window);
    swapchain->create();

    renderPass =
        new VulkanRenderPass(*vulkanContext, swapchain->getImageFormat());
    renderPass->createGBufferRenderPass();
    renderPass->createSSAORenderPass(); // We can create the render pass even if
                                        // we don't use it
    renderPass->create();               // Final render pass

    swapchain->createFramebuffers(renderPass->getFinalRenderPass());

    pipeline = new VulkanPipeline(*vulkanContext);
    pipeline->createGraphicsPipeline(
        renderPass->getGBufferRenderPass(), "shaders/default.vert.spv",
        "shaders/color.frag.spv", swapchain->getExtent(),
        4 // 4 Color Attachments for G-Buffer
    );

    descriptorManager = new VulkanDescriptorManager(*vulkanContext);
    descriptorManager->createDescriptorPool(100);

    renderSystem = new VulkanRenderSystem(*vulkanContext, *descriptorManager,
                                          *pipeline, entity_manager);
    renderSystem->initGBuffer(renderPass->getGBufferRenderPass(),
                              swapchain->getExtent());

    // Initialize SSAO
    ssao = new VulkanSSAO(*vulkanContext);
    ssao->init(renderPass->getSSAORenderPass(), swapchain->getExtent());

    // Create Composition Pipeline
    compPipeline = new VulkanPipeline(*vulkanContext);
    compPipeline->createCompositionPipeline(
        renderPass->getFinalRenderPass(), "shaders/composite.vert.spv",
        "shaders/composite.frag.spv", swapchain->getExtent());

    compDescriptorSet = descriptorManager->allocateDescriptorSet(
        compPipeline->getDescriptorSetLayout());

    // Update Composition Descriptor Set (5 bindings: 4 G-Buffer + 1 SSAO)
    descriptorManager->updateTextureDescriptor(
        compDescriptorSet, 0,
        renderSystem->getGBuffer().colorRT->getImageView(),
        renderSystem->getGBuffer().colorRT->getSampler());
    descriptorManager->updateTextureDescriptor(
        compDescriptorSet, 1,
        renderSystem->getGBuffer().normalRT->getImageView(),
        renderSystem->getGBuffer().normalRT->getSampler());
    descriptorManager->updateTextureDescriptor(
        compDescriptorSet, 2,
        renderSystem->getGBuffer().depthRT->getImageView(),
        renderSystem->getGBuffer().depthRT->getSampler());
    descriptorManager->updateTextureDescriptor(
        compDescriptorSet, 3,
        renderSystem->getGBuffer().materialRT->getImageView(),
        renderSystem->getGBuffer().materialRT->getSampler());
    descriptorManager->updateTextureDescriptor(compDescriptorSet, 4,
                                               ssao->ssaoOutput->getImageView(),
                                               ssao->ssaoOutput->getSampler());

    // Bind lighting UBO to composition descriptor set
    VkDescriptorBufferInfo lightingBufferInfo{};
    lightingBufferInfo.buffer = renderSystem->getLightingUBO()->getBuffer();
    lightingBufferInfo.offset = 0;
    lightingBufferInfo.range = sizeof(LightingUBO);

    VkWriteDescriptorSet lightingWrite{};
    lightingWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    lightingWrite.dstSet = compDescriptorSet;
    lightingWrite.dstBinding = 5;
    lightingWrite.dstArrayElement = 0;
    lightingWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    lightingWrite.descriptorCount = 1;
    lightingWrite.pBufferInfo = &lightingBufferInfo;

    vkUpdateDescriptorSets(vulkanContext->getDevice(), 1, &lightingWrite, 0,
                           nullptr);

    // Update SSAO Descriptor Set
    VkDescriptorSet ssaoSet =
        descriptorManager->allocateDescriptorSet(ssao->descriptorSetLayout);
    ssao->updateDescriptorSet(ssaoSet, renderSystem->getGBuffer().depthRT,
                              renderSystem->getGBuffer().normalRT);

    createCommandBuffers();
    createSyncObjects();
    initImGui();

    // Initialize default lights
    initializeLights();

    std::cout << "Vulkan initialized successfully!" << std::endl;
  }

  void createCommandBuffers() {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = vulkanContext->getCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(vulkanContext->getDevice(), &allocInfo,
                                 commandBuffers.data()) != VK_SUCCESS) {
      throw std::runtime_error("failed to allocate command buffers!");
    }
  }

  void createSyncObjects() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      if (vkCreateSemaphore(vulkanContext->getDevice(), &semaphoreInfo, nullptr,
                            &imageAvailableSemaphores[i]) != VK_SUCCESS ||
          vkCreateSemaphore(vulkanContext->getDevice(), &semaphoreInfo, nullptr,
                            &renderFinishedSemaphores[i]) != VK_SUCCESS ||
          vkCreateFence(vulkanContext->getDevice(), &fenceInfo, nullptr,
                        &inFlightFences[i]) != VK_SUCCESS) {
        throw std::runtime_error("failed to create synchronization objects!");
      }
    }
  }

  void initImGui() {
    // Create descriptor pool for ImGui
    VkDescriptorPoolSize pool_sizes[] = {
        {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    if (vkCreateDescriptorPool(vulkanContext->getDevice(), &pool_info, nullptr,
                               &imguiDescriptorPool) != VK_SUCCESS) {
      throw std::runtime_error("Failed to create ImGui descriptor pool!");
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = vulkanContext->getInstance();
    init_info.PhysicalDevice = vulkanContext->getPhysicalDevice();
    init_info.Device = vulkanContext->getDevice();
    init_info.QueueFamily =
        vulkanContext->getQueueFamilies().graphicsFamily.value();
    init_info.Queue = vulkanContext->getGraphicsQueue();
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = imguiDescriptorPool;
    init_info.Subpass = 0;
    init_info.MinImageCount = 2;
    init_info.ImageCount = swapchain->getImages().size();
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = nullptr;
    init_info.CheckVkResultFn = nullptr;

    ImGui_ImplVulkan_Init(&init_info, renderPass->getFinalRenderPass());

    // Upload Fonts
    VkCommandBuffer command_buffer = vulkanContext->beginSingleTimeCommands();
    ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
    vulkanContext->endSingleTimeCommands(command_buffer);
    ImGui_ImplVulkan_DestroyFontUploadObjects();

    std::cout << "ImGui initialized successfully!" << std::endl;
  }

  void initializeLights() {
    // Initialize default lights using LightingManager
    lightingManager.initializeDefaultLights();

    // Create DebugUI instance now that entity_manager exists
    debugUI = std::make_unique<dunkan::DebugUI>(config, lightingManager);
  }

  void updateLightingUBO() {
    // Update SSAO parameters from config
    ssao->updateParameters(config.ssaoRadius, config.ssaoBias,
                           config.ssaoPower);

    // Update lighting UBO using LightingManager
    glm::vec3 viewPos = glm::vec3(960.0f, 540.0f, 10.0f);
    lightingManager.updateLightingUBO(renderSystem->getLightingUBO(),
                                      config.ambientLight, viewPos);
  }

  void renderDebugUI() {
    // Start ImGui frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Rebuild entity cache only when needed (on first frame or after entity
    // changes)
    if (entityCacheNeedsRebuild) {
      entityEditCache.clear();
      entity_manager.foreach<VulkanRenderSystem_c, VulkanRenderSystem_t>(
          [&](Entity &, RenderComponent &renderComp,
              PhysicsComponent &physicsComp) {
            entityEditCache.push_back(
                {&renderComp, &physicsComp, renderComp.albedoTextureName});
          });
      entityCacheNeedsRebuild = false;
    }

    // Render debug UI using component
    debugUI->render(m_fps, entity_manager.get_entities_count(),
                    entityEditCache);

    // Finish ImGui frame
    ImGui::Render();
  }

  void loadGameEntities() {
    // Mark cache for rebuild after loading entities
    std::cout << "Loading game entities..." << std::endl;

    try {
      // Load all textures first
      std::cout << "Loading textures from data folder..." << std::endl;

      // Abbey textures (albedo, depth, normal, no material)
      std::cout << "Loading Abbey textures..." << std::endl;
      renderSystem->loadTexture("abbey_albedo", "data/abbey_albedo.png",
                                "data/abbey_height.png", "data/abbey_normal.png", "");

      // Tree textures (albedo, depth, normal, material)
      renderSystem->loadTexture("tree_albedo", "data/tree_albedo.png",
                                "data/tree_height.png", "data/tree_normal.png", "data/tree_material.png");

      // Teapot textures (albedo, depth, normal, no material)
      renderSystem->loadTexture("teapot_albedo", "data/teapot_albedo.png",
                                "data/teapot_height.png", "data/teapot_normal.png", "");

      // Torus textures (albedo, depth, normal, material)
      renderSystem->loadTexture("torus_albedo", "data/torus_albedo.png",
                                "data/torus_height.png", "data/torus_normal.png", "data/torus_material.png");

      // Ground (wetsand) textures (albedo, depth, normal, material)
      renderSystem->loadTexture("wetsand_albedo", "data/wetsand_albedo.png",
                                "data/wetsand_height.png", "data/wetsand_normal.png", "data/wetsand_material.png");

      std::cout << "Textures loaded successfully!" << std::endl;

      // Create Abbey entity
      Entity &abbey = entity_manager.create_entity();
      entity_manager.add_component<PhysicsComponent>(
          abbey, PhysicsComponent{.x = 800.f, .y = 400.f, .z = 0.7f});
      entity_manager.add_component<RenderComponent>(
          abbey,
          RenderComponent{nullptr, // Will be set by texture name lookup
                          glm::vec4(0, 0, 1024, 1024), // Texture rect
                          10.0f,                       // height
                          1.0f,                        // scale
                          "abbey_albedo", "abbey_normal", "abbey_height"});

      // Create Trees (3 trees scattered around)
      for (int i = 0; i < 3; i++) {
        Entity &tree = entity_manager.create_entity();
        entity_manager.add_component<PhysicsComponent>(
            tree, PhysicsComponent{.x = 300.f + i * 400.f,
                                   .y = 600.f + (i % 2) * 100.f,
                                   .z = 0.6f});
        entity_manager.add_component<RenderComponent>(
            tree, RenderComponent{nullptr, glm::vec4(0, 0, 256, 512), 12.0f,
                                  1.0f, "tree_albedo", "tree_normal",
                                  "tree_height", "tree_material"});
      }

      // Create Teapot entity
      Entity &teapot = entity_manager.create_entity();
      entity_manager.add_component<PhysicsComponent>(
          teapot, PhysicsComponent{.x = 1200.f, .y = 300.f, .z = 0.5f});
      entity_manager.add_component<RenderComponent>(
          teapot,
          RenderComponent{nullptr, glm::vec4(0, 0, 200, 200), 8.0f, 1.0f,
                          "teapot_albedo", "teapot_normal", "teapot_height"});

      // Create Torus entity
      Entity &torus = entity_manager.create_entity();
      entity_manager.add_component<PhysicsComponent>(
          torus, PhysicsComponent{.x = 500.f, .y = 200.f, .z = 0.4f});
      entity_manager.add_component<RenderComponent>(
          torus, RenderComponent{nullptr, glm::vec4(0, 0, 180, 180), 7.0f, 1.0f,
                                 "torus_albedo", "torus_normal", "torus_height",
                                 "torus_material"});

      // Create Ground plane
      Entity &ground = entity_manager.create_entity();
      entity_manager.add_component<PhysicsComponent>(
          ground, PhysicsComponent{
                      .x = 10.f, // Center of 1920
                      .y = 70.f,
                      .z = -1.0f // Bottom layer
                  });
      entity_manager.add_component<RenderComponent>(
          ground, RenderComponent{
                      nullptr, glm::vec4(0, 0, 512, 512), // Large ground tile
                      1.0f,
                      1.5f, // Scaled up
                      "wetsand_albedo", "wetsand_normal", "wetsand_height",
                      "wetsand_material"});

      std::cout << "Entities created: " << entity_manager.get_entities_count()
                << std::endl;

    } catch (const std::exception &e) {
      std::cerr << "Error loading game entities: " << e.what() << std::endl;
    }
  }

  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }

    // 1. G-Buffer Pass (Off-screen)
    renderSystem->prepareFrame(commandBuffer, currentFrame);
    renderSystem->renderEntities(commandBuffer, currentFrame);

    // 2. SSAO Pass (Off-screen) - Only if enabled
    if (config.enableSSAO) {
      VkRenderPassBeginInfo ssaoPassInfo{};
      ssaoPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      ssaoPassInfo.renderPass = renderPass->getSSAORenderPass();
      ssaoPassInfo.framebuffer = ssao->framebuffer;
      ssaoPassInfo.renderArea.offset = {0, 0};
      ssaoPassInfo.renderArea.extent = swapchain->getExtent();

      VkClearValue clearValue = {{0.0f, 0.0f, 0.0f, 1.0f}};
      ssaoPassInfo.clearValueCount = 1;
      ssaoPassInfo.pClearValues = &clearValue;

      vkCmdBeginRenderPass(commandBuffer, &ssaoPassInfo,
                           VK_SUBPASS_CONTENTS_INLINE);

      vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                        ssao->pipeline);
      vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                              ssao->pipelineLayout, 0, 1, &ssao->descriptorSet,
                              0, nullptr);

      // Update SSAO uniforms (projection matrix)
      glm::mat4 projection =
          glm::ortho(0.0f, 1920.0f, 1080.0f, 0.0f, -100.0f, 100.0f);
      ssao->update(commandBuffer, projection);

      vkCmdDraw(commandBuffer, 3, 1, 0, 0); // Full screen triangle

      vkCmdEndRenderPass(commandBuffer);
    }

    // 3. Final Composition Pass (To Swapchain)
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass->getFinalRenderPass();
    renderPassInfo.framebuffer = swapchain->getFramebuffers()[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapchain->getExtent();

    VkClearValue clearColor = {{{0.1f, 0.1f, 0.15f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    // Render full screen quad combining G-Buffer attachments
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      compPipeline->getPipeline());

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            compPipeline->getLayout(), 0, 1, &compDescriptorSet,
                            0, nullptr);

    // Push constants for debug view mode, SSAO enable, and gamma
    struct {
      int debugViewMode;
      int enableSSAO;
      float gamma;
    } pushConstants;
    pushConstants.debugViewMode = static_cast<int>(config.currentDebugView);
    pushConstants.enableSSAO = config.enableSSAO ? 1 : 0;
    pushConstants.gamma = config.gammaCorrection;

    vkCmdPushConstants(commandBuffer, compPipeline->getLayout(),
                       VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(pushConstants),
                       &pushConstants);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0); // Full screen triangle

    // Render ImGui on top
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }
  }

  void drawFrame() {
    vkWaitForFences(vulkanContext->getDevice(), 1,
                    &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        vulkanContext->getDevice(), swapchain->getSwapchain(), UINT64_MAX,
        imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
      swapchain->recreate();
      swapchain->createFramebuffers(renderPass->getFinalRenderPass());
      return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      throw std::runtime_error("failed to acquire swap chain image!");
    }

    vkResetFences(vulkanContext->getDevice(), 1, &inFlightFences[currentFrame]);

    // Build ImGui UI for this frame
    renderDebugUI();

    vkResetCommandBuffer(commandBuffers[currentFrame], 0);
    recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(vulkanContext->getGraphicsQueue(), 1, &submitInfo,
                      inFlightFences[currentFrame]) != VK_SUCCESS) {
      throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapchain->getSwapchain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(vulkanContext->getPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
      swapchain->recreate();
      swapchain->createFramebuffers(renderPass->getFinalRenderPass());
    } else if (result != VK_SUCCESS) {
      throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
  }

  void mainLoop() {
    auto start_time = std::chrono::high_resolution_clock::now();
    auto last_fps_time = std::chrono::high_resolution_clock::now();
    auto last_frame_time = std::chrono::high_resolution_clock::now();
    int frame_count = 0;

    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
      
      // Calculate delta time for animations
      auto current_time = std::chrono::high_resolution_clock::now();
      float deltaTime = std::chrono::duration<float>(current_time - last_frame_time).count();
      last_frame_time = current_time;
      
      // Update animated lights (spotlights)
      lightingManager.updateAnimatedLights(deltaTime);

      // Update lighting UBO each frame from ImGui state
      updateLightingUBO();

      drawFrame();

      frame_count++;
      auto elapsed_fps = std::chrono::duration_cast<std::chrono::seconds>(
                             current_time - last_fps_time)
                             .count();
      if (elapsed_fps >= 1) {
        m_fps = frame_count;
        frame_count = 0;
        last_fps_time = current_time;
      }
    }

    vkDeviceWaitIdle(vulkanContext->getDevice());
  }

  void cleanup() {
    vkDeviceWaitIdle(vulkanContext->getDevice());

    // Cleanup ImGui
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (imguiDescriptorPool != VK_NULL_HANDLE) {
      vkDestroyDescriptorPool(vulkanContext->getDevice(), imguiDescriptorPool,
                              nullptr);
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
      vkDestroySemaphore(vulkanContext->getDevice(),
                         renderFinishedSemaphores[i], nullptr);
      vkDestroySemaphore(vulkanContext->getDevice(),
                         imageAvailableSemaphores[i], nullptr);
      vkDestroyFence(vulkanContext->getDevice(), inFlightFences[i], nullptr);
    }

    delete ssao;
    delete renderSystem;
    delete descriptorManager;
    delete pipeline;
    delete compPipeline;
    delete renderPass;
    delete swapchain;
    delete vulkanContext;

    glfwDestroyWindow(window);
    glfwTerminate();
  }
};

int main() {
  VulkanApplication app;

  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
