#include "vulkan/VulkanBuffer.hpp"
#include <cstring>
#include <stdexcept>

VulkanBuffer::VulkanBuffer(VulkanContext& context) : m_context(context) {
}

VulkanBuffer::~VulkanBuffer() {
    cleanup();
}

void VulkanBuffer::create(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    m_size = size;
    
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    if (vkCreateBuffer(m_context.getDevice(), &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }
    
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_context.getDevice(), m_buffer, &memRequirements);
    
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = m_context.findMemoryType(memRequirements.memoryTypeBits, properties);
    
    if (vkAllocateMemory(m_context.getDevice(), &allocInfo, nullptr, &m_memory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }
    
    vkBindBufferMemory(m_context.getDevice(), m_buffer, m_memory, 0);
}

void VulkanBuffer::cleanup() {
    if (m_buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(m_context.getDevice(), m_buffer, nullptr);
        m_buffer = VK_NULL_HANDLE;
    }
    if (m_memory != VK_NULL_HANDLE) {
        vkFreeMemory(m_context.getDevice(), m_memory, nullptr);
        m_memory = VK_NULL_HANDLE;
    }
}

void VulkanBuffer::copyFrom(const void* data, VkDeviceSize size) {
    void* mappedData = map();
    memcpy(mappedData, data, static_cast<size_t>(size));
    unmap();
}

void VulkanBuffer::copyTo(VulkanBuffer& dst, VkDeviceSize size) {
    VkCommandBuffer commandBuffer = m_context.beginSingleTimeCommands();
    
    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, m_buffer, dst.getBuffer(), 1, &copyRegion);
    
    m_context.endSingleTimeCommands(commandBuffer);
}

void* VulkanBuffer::map() {
    void* data;
    vkMapMemory(m_context.getDevice(), m_memory, 0, m_size, 0, &data);
    return data;
}

void VulkanBuffer::unmap() {
    vkUnmapMemory(m_context.getDevice(), m_memory);
}
