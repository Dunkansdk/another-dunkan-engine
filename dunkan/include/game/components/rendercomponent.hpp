#pragma once

#include <cstddef>
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include "vulkan/VulkanTypes.hpp"
#include "vulkan/VulkanImage.hpp"

struct RenderComponent {

    RenderComponent() = default;

    RenderComponent(VulkanImage* albedo, const glm::vec4& rectangle, float height, float scale) 
    {
        m_texture = albedo;
        this->height = height;
        this->scale = scale;
        this->textureRect = rectangle;
    }

    RenderComponent(VulkanImage* albedo, const glm::vec4& rectangle, float height, float scale, 
                    VulkanImage* normal, VulkanImage* depth) 
    {
        m_texture = albedo;
        m_normal = normal;
        m_depth = depth;
        this->height = height;
        this->scale = scale;
        this->textureRect = rectangle;
    }

    RenderComponent(VulkanImage* albedo, const glm::vec4& rectangle, float height, float scale,  
                    VulkanImage* normal, VulkanImage* depth, VulkanImage* material) 
    {
        m_texture = albedo;
        m_normal = normal;
        m_depth = depth;
        m_material = material;
        this->height = height;
        this->scale = scale;
        this->textureRect = rectangle;
    }

    RenderComponent(VulkanImage* albedo, const glm::vec4& rectangle, float height, float scale,  
                    VulkanImage* normal, VulkanImage* depth, float roughness, float metalness, float translucency) 
    {
        m_texture = albedo;
        m_normal = normal;
        m_depth = depth;
        this->height = height;
        this->scale = scale;
        this->textureRect = rectangle;
        this->roughness = roughness;
        this->metalness = metalness;
        this->translucency = translucency;
    }

    RenderComponent(VulkanImage* albedo, const glm::vec4& rectangle, float height, float scale,  
                    VulkanImage* normal, VulkanImage* depth, VulkanImage* material, bool moveable) 
    {
        m_texture = albedo;
        m_normal = normal;
        m_depth = depth;
        m_material = material;
        this->height = height;
        this->scale = scale;
        this->textureRect = rectangle;
        this->moveable = moveable;
    }

    
    // Constructor with texture name strings (for deferred texture loading)
    RenderComponent(VulkanImage* albedo, const glm::vec4& rectangle, float height, float scale,
                    const std::string& albedoName, const std::string& normalName = "", 
                    const std::string& depthName = "", const std::string& materialName = "")
    {
        m_texture = albedo;
        this->height = height;
        this->scale = scale;
        this->textureRect = rectangle;
        this->albedoTextureName = albedoName;
        this->normalTextureName = normalName;
        this->depthTextureName = depthName;
        this->materialTextureName = materialName;
    }

    RenderComponent& load() {
        // Prepare vertex data for quad rendering
        return *this;
    }

    // Position and transform
    glm::vec2 position{0.0f, 0.0f};
    glm::vec2 scaleVec{1.0f, 1.0f};
    glm::vec4 textureRect{0, 0, 0, 0};  // x, y, width, height
    
    float height {10.0f};
    float scale {1.0f};
    bool is_selected{false};
    bool moveable{true};
    
    VulkanImage* m_texture{nullptr};
    VulkanImage* m_depth{nullptr};
    VulkanImage* m_normal{nullptr};
    VulkanImage* m_material{nullptr};
    
    
    float roughness{0.f};
    float metalness{0.f};
    float translucency{0.f};
    
    // Texture names for deferred loading
    std::string albedoTextureName;
    std::string normalTextureName;
    std::string depthTextureName;
    std::string materialTextureName;

};
