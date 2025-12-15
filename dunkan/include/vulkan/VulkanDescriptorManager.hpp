#pragma once

#include <vulkan/vulkan.h>
#include "VulkanContext.hpp"
#include "VulkanBuffer.hpp"
#include <vector>

class VulkanDescriptorManager {
public:
    VulkanDescriptorManager(VulkanContext& context);
    ~VulkanDescriptorManager();
    
    void createDescriptorPool(uint32_t maxSets);
    VkDescriptorSet allocateDescriptorSet(VkDescriptorSetLayout layout);
    void updateTextureDescriptor(VkDescriptorSet descriptorSet, uint32_t binding,
                                  VkImageView imageView, VkSampler sampler);
    void updateUniformBuffer(VkDescriptorSet descriptorSet, uint32_t binding,
                             VkBuffer buffer, VkDeviceSize size);
    
    void cleanup();
    
    VkDescriptorPool getPool() const { return m_descriptorPool; }
    
private:
    VulkanContext& m_context;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
};
