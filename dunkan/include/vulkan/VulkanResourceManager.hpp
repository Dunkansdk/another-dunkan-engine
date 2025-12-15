#pragma once

#include <vulkan/vulkan.h>
#include "VulkanContext.hpp"
#include "VulkanImage.hpp"
#include <map>
#include <string>
#include <memory>

class VulkanResourceManager {
public:
    explicit VulkanResourceManager(VulkanContext& context);
    ~VulkanResourceManager();
    
    void load(const std::string& id, const std::string& filename);
    VulkanImage* get(const std::string& id);
    VulkanImage* getPointer(const std::string& id);
    
    void cleanup();
    
private:
    VulkanContext& m_context;
    std::map<std::string, std::unique_ptr<VulkanImage>> m_textureStorage;
};
