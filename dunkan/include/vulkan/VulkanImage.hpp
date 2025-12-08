#pragma once

#include <vulkan/vulkan.h>
#include "VulkanContext.hpp"
#include <string>

class VulkanImage {
public:
    VulkanImage(VulkanContext& context);
    ~VulkanImage();
    
    void createImage(uint32_t width, uint32_t height, VkFormat format, 
                     VkImageTiling tiling, VkImageUsageFlags usage, 
                     VkMemoryPropertyFlags properties);
    
    void createRenderTarget(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage);
    void createImageView(VkFormat format, VkImageAspectFlags aspectFlags);
    void createSampler();
    void transitionLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyFromBuffer(VkBuffer buffer, uint32_t width, uint32_t height);
    void loadFromFile(const std::string& filepath);
    
    void cleanup();
    
    VkImage getImage() const { return m_image; }
    VkImageView getImageView() const { return m_imageView; }
    VkSampler getSampler() const { return m_sampler; }
    uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height; }
    
private:
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    
    VulkanContext& m_context;
    VkImage m_image = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;
    VkImageView m_imageView = VK_NULL_HANDLE;
    VkSampler m_sampler = VK_NULL_HANDLE;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
};
