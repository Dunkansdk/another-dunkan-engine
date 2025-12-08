#include "vulkan/VulkanDescriptorManager.hpp"
#include <stdexcept>
#include <array>

VulkanDescriptorManager::VulkanDescriptorManager(VulkanContext& context)
    : m_context(context) {
}

VulkanDescriptorManager::~VulkanDescriptorManager() {
    cleanup();
}

void VulkanDescriptorManager::createDescriptorPool(uint32_t maxSets) {
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = maxSets;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = maxSets * 4; // Allow multiple textures per set
    
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = maxSets;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    
    if (vkCreateDescriptorPool(m_context.getDevice(), &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

VkDescriptorSet VulkanDescriptorManager::allocateDescriptorSet(VkDescriptorSetLayout layout) {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;
    
    VkDescriptorSet descriptorSet;
    if (vkAllocateDescriptorSets(m_context.getDevice(), &allocInfo, &descriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor set!");
    }
    
    return descriptorSet;
}

void VulkanDescriptorManager::updateTextureDescriptor(VkDescriptorSet descriptorSet, uint32_t binding,
                                                       VkImageView imageView, VkSampler sampler) {
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = imageView;
    imageInfo.sampler = sampler;
    
    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfo;
    
    vkUpdateDescriptorSets(m_context.getDevice(), 1, &descriptorWrite, 0, nullptr);
}

void VulkanDescriptorManager::updateUniformBuffer(VkDescriptorSet descriptorSet, uint32_t binding,
                                                   VkBuffer buffer, VkDeviceSize size) {
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = size;
    
    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;
    
    vkUpdateDescriptorSets(m_context.getDevice(), 1, &descriptorWrite, 0, nullptr);
}

void VulkanDescriptorManager::cleanup() {
    if (m_descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(m_context.getDevice(), m_descriptorPool, nullptr);
        m_descriptorPool = VK_NULL_HANDLE;
    }
}
