#pragma once

#include <map>

enum ShadowCastingType {
    NO_SHADOW,
    DIRECTIONNAL,
    DYNAMIC,
    ALL_SHADOWS
};

struct LightComponent;  // Forward declaration

struct ShadowComponent {

    ShadowComponent() {}

    ShadowComponent(ShadowCastingType type) {
        this->shadow_type = type;
    }
    
    ShadowCastingType shadow_type{};
    
    // TODO: Implement shadow maps with Vulkan textures
    // std::map<LightComponent*, VulkanImage*> m_shadow_map{};

};