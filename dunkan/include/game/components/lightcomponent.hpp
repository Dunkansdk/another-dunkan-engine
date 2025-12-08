#pragma once

#include <glm/glm.hpp>
#include "vulkan/VulkanTypes.hpp"

enum LightType {
    SPOT, 
    DIRECTIONAL, 
    OMNI
};

struct LightComponent {

    LightType light_type{LightType::DIRECTIONAL};

    Color4 diffuse_color {Color4::White};
    Color4 specular_color {Color4::White};

    glm::vec3 direction {0.0f, 0.0f, -1.0f};

    float radius{100.f};
    float intensity{1.f};
    
    float quadratic_attenuation {1.f};
    float linear_attenuation {0.0f};

    bool is_selected{false};

};