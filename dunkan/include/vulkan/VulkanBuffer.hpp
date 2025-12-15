#pragma once

#include <vulkan/vulkan.h>
#include "VulkanContext.hpp"

class VulkanBuffer {
public:
    VulkanBuffer(VulkanContext& context);
    ~VulkanBuffer();
    
    void create(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    void cleanup();
    
    void copyFrom(const void* data, VkDeviceSize size);
    void copyTo(VulkanBuffer& dst, VkDeviceSize size);
    
    VkBuffer getBuffer() const { return m_buffer; }
    VkDeviceMemory getMemory() const { return m_memory; }
    VkDeviceSize getSize() const { return m_size; }
    
    void* map();
    void unmap();
    
private:
    VulkanContext& m_context;
    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;
    VkDeviceSize m_size = 0;
};
