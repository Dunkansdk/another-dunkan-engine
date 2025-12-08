#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>
#include <string>
#include "vulkan/VulkanContext.hpp"
#include "vulkan/VulkanBuffer.hpp"
#include "vulkan/VulkanDescriptorManager.hpp"
#include "vulkan/VulkanPipeline.hpp"
#include "vulkan/VulkanImage.hpp"
#include "vulkan/VulkanGBuffer.hpp"
#include "game/types.hpp"

class VulkanRenderSystem {
public:
    VulkanRenderSystem(VulkanContext& context, VulkanDescriptorManager& descriptorManager,
                       VulkanPipeline& pipeline, EntityManager& entityManager);
    ~VulkanRenderSystem();
    
    void prepareFrame(VkCommandBuffer commandBuffer, uint32_t frameIndex);
    void renderEntities(VkCommandBuffer commandBuffer, uint32_t frameIndex);
    
    void initGBuffer(VkRenderPass renderPass, VkExtent2D extent);
    const GBuffer& getGBuffer() const { return m_gbuffer; }
    VulkanBuffer* getLightingUBO() { return m_lightingUBO; }
    
    void createDefaultTexture();
    VulkanImage* getDefaultTexture() { return m_defaultTexture; }
    
    // Texture management
    void loadTexture(const std::string& name, const std::string& filepath, 
                    const std::string& depthFilepath = "",
                    const std::string& normalFilepath = "",
                    const std::string& materialFilepath = "");
    VulkanImage* getTexture(const std::string& name);
    
private:
    struct SpriteData {
        std::vector<Vertex> vertices;
        VulkanBuffer* vertexBuffer = nullptr;
        VulkanImage* texture = nullptr;
        glm::vec2 position;
        glm::vec2 size;
    };
    
    void createQuadVertices(std::vector<Vertex>& vertices, glm::vec2 size);
    
    VulkanContext& m_context;
    VulkanDescriptorManager& m_descriptorManager;
    GBuffer m_gbuffer;
    VulkanPipeline& m_pipeline;
    EntityManager& m_entityManager;
    
    VulkanImage* m_defaultTexture = nullptr;
    VulkanBuffer* m_quadVertexBuffer = nullptr;
    std::vector<VkDescriptorSet> m_descriptorSets;
    VulkanBuffer* m_uniformBuffer = nullptr;
    VulkanBuffer* m_lightingUBO = nullptr;  // Lighting uniform buffer
    std::unordered_map<std::string, VulkanImage*> m_textures;
    std::unordered_map<std::string, VkDescriptorSet> m_textureDescriptorSets; // One descriptor set per texture
    
    static constexpr int MAX_FRAMES = 2;
};
