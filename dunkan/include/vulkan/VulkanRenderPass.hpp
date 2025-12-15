#pragma once

#include <vulkan/vulkan.h>
#include "VulkanContext.hpp"
#include <vector>

class VulkanRenderPass {
public:
    VulkanRenderPass(VulkanContext& context, VkFormat swapchainFormat);
    ~VulkanRenderPass();
    
    void create();
    void createGBufferRenderPass();
    void createSSAORenderPass();
    void cleanup();
    
    VkRenderPass getFinalRenderPass() const { return m_finalRenderPass; }
    VkRenderPass getGBufferRenderPass() const { return m_gbufferRenderPass; }
    VkRenderPass getSSAORenderPass() const { return m_ssaoRenderPass; }
    
private:
    VulkanContext& m_context;
    VkFormat m_swapchainFormat;
    VkRenderPass m_finalRenderPass = VK_NULL_HANDLE;
    VkRenderPass m_gbufferRenderPass = VK_NULL_HANDLE;
    VkRenderPass m_ssaoRenderPass = VK_NULL_HANDLE;
};
