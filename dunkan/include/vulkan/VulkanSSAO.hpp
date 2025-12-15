#pragma once

#include "VulkanContext.hpp"
#include "VulkanImage.hpp"
#include "VulkanBuffer.hpp"
#include <glm/glm.hpp>
#include <random>
#include <vector>

class VulkanSSAO {
public:
    VulkanSSAO(VulkanContext& context);
    ~VulkanSSAO();

    void init(VkRenderPass renderPass, VkExtent2D extent);
    void cleanup();
    
    void update(VkCommandBuffer commandBuffer, const glm::mat4& projection);
    void updateParameters(float radius, float bias, float power);
    void updateDescriptorSet(VkDescriptorSet descriptorSet, VulkanImage* depth, VulkanImage* normal);

    VkFramebuffer framebuffer;
    VulkanImage* ssaoOutput;
    
    VkDescriptorSet descriptorSet;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;

private:
    void createNoiseTexture();
    void createKernel();
    void createPipeline(VkRenderPass renderPass, VkExtent2D extent);
    
    VulkanContext& m_context;
    VulkanImage* m_noiseTexture;
    VulkanBuffer* m_kernelBuffer;
    
    struct SSAOKernel {
        glm::mat4 projection;
        glm::vec4 samples[64];
        float radius;
        float bias;
        float power;
        float _padding;
    } m_uboData;
};
