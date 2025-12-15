#pragma once

#include "vulkan/VulkanContext.hpp"
#include "vulkan/VulkanImage.hpp"
#include <vulkan/vulkan.h>

// G-Buffer: 4 render targets for deferred rendering
struct GBuffer {
    VulkanImage* colorRT = nullptr;      // RGBA8 - Albedo texture
    VulkanImage* normalRT = nullptr;     // RGBA16F - World-space normals + roughness
    VulkanImage* depthRT = nullptr;      // R32F - Parallax height encoding
    VulkanImage* materialRT = nullptr;   // RGBA8 - Roughness, metalness, translucency, AO
    VulkanImage* depthStencilImage = nullptr; // Actual depth buffer for Z-testing
    
    VkFramebuffer framebuffer = VK_NULL_HANDLE;
    VkRenderPass renderPass = VK_NULL_HANDLE;
    
    uint32_t width = 0;
    uint32_t height = 0;
    
    void create(VulkanContext& context, uint32_t w, uint32_t h);
    void cleanup(VulkanContext& context);
    void recreate(VulkanContext& context, uint32_t w, uint32_t h);
};
