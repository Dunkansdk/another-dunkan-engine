#pragma once

#include <vulkan/vulkan.h>
#include "VulkanContext.hpp"
#include "VulkanTypes.hpp"
#include <vector>
#include <string>

class VulkanPipeline {
public:
    VulkanPipeline(VulkanContext& context);
    ~VulkanPipeline();
    
    void createGraphicsPipeline(
        VkRenderPass renderPass,
        const std::string& vertShaderPath,
        const std::string& fragShaderPath,
        VkExtent2D extent,
        uint32_t attachmentCount = 1);
        
    void createCompositionPipeline(
        VkRenderPass renderPass,
        const std::string& vertShaderPath,
        const std::string& fragShaderPath,
        VkExtent2D extent);
        
    void cleanup();
        
    VkDescriptorSetLayout getDescriptorSetLayout() const { return m_descriptorSetLayout; }
    VkPipeline getPipeline() const { return m_pipeline; }
    VkPipelineLayout getLayout() const { return m_pipelineLayout; }
    
private:
    VkShaderModule createShaderModule(const std::vector<char>& code);
    std::vector<char> readFile(const std::string& filename);
    
    VulkanContext& m_context;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
};
