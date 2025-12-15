#include "vulkan/VulkanGBuffer.hpp"
#include <stdexcept>

void GBuffer::create(VulkanContext& context, uint32_t w, uint32_t h) {
    width = w;
    height = h;
    
    // Create Color RT (RGBA8 SRGB)
    colorRT = new VulkanImage(context);
    colorRT->createRenderTarget(width, height, VK_FORMAT_R8G8B8A8_SRGB,
                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    colorRT->createImageView(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
    colorRT->createSampler();
    
    // Create Normal RT (RGBA16F for precision)
    normalRT = new VulkanImage(context);
    normalRT->createRenderTarget(width, height, VK_FORMAT_R16G16B16A16_SFLOAT,
                                 VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    normalRT->createImageView(VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT);
    normalRT->createSampler();
    
    // Create Depth RT (R32F for encoded height)
    depthRT = new VulkanImage(context);
    depthRT->createRenderTarget(width, height, VK_FORMAT_R32_SFLOAT,
                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    depthRT->createImageView(VK_FORMAT_R32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT);
    depthRT->createSampler();
    
    // Create Material RT (RGBA8)
    materialRT = new VulkanImage(context);
    materialRT->createRenderTarget(width, height, VK_FORMAT_R8G8B8A8_UNORM,
                                   VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    materialRT->createImageView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
    materialRT->createSampler();
    
    // Create Depth/Stencil Image (Actual Depth Buffer)
    depthStencilImage = new VulkanImage(context);
    depthStencilImage->createRenderTarget(width, height, context.findDepthFormat(),
                                          VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    depthStencilImage->createImageView(context.findDepthFormat(), VK_IMAGE_ASPECT_DEPTH_BIT);
}

void GBuffer::cleanup(VulkanContext& context) {
    if (framebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(context.getDevice(), framebuffer, nullptr);
        framebuffer = VK_NULL_HANDLE;
    }
    
    delete colorRT;
    delete normalRT;
    delete depthRT;
    delete materialRT;
    delete depthStencilImage;
    
    colorRT = normalRT = depthRT = materialRT = depthStencilImage = nullptr;
}

void GBuffer::recreate(VulkanContext& context, uint32_t w, uint32_t h) {
    cleanup(context);
    create(context, w, h);
}
