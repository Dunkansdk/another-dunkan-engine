#pragma once

#include <vulkan/vulkan.h>
#include "VulkanContext.hpp"
#include <vector>
#include <GLFW/glfw3.h>

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanSwapchain {
public:
    VulkanSwapchain(VulkanContext& context, GLFWwindow* window);
    ~VulkanSwapchain();
    
    void create();
    void cleanup();
    void recreate();
    
    VkSwapchainKHR getSwapchain() const { return m_swapchain; }
    VkFormat getImageFormat() const { return m_imageFormat; }
    VkExtent2D getExtent() const { return m_extent; }
    const std::vector<VkImage>& getImages() const { return m_images; }
    const std::vector<VkImageView>& getImageViews() const { return m_imageViews; }
    const std::vector<VkFramebuffer>& getFramebuffers() const { return m_framebuffers; }
    
    void createFramebuffers(VkRenderPass renderPass);
    
    static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
    
private:
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    
    VulkanContext& m_context;
    GLFWwindow* m_window;
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
    std::vector<VkFramebuffer> m_framebuffers;
    VkFormat m_imageFormat;
    VkExtent2D m_extent;
};
