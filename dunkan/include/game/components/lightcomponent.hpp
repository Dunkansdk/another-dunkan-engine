#pragma once

#include <SFML/Graphics.hpp>

enum LightType {
    SPOT, 
    DIRECTIONAL, 
    OMNI
};

struct LightComponent {

    LightType light_type{LightType::DIRECTIONAL};

    sf::Color diffuse_color {sf::Color::White};
    sf::Color specular_color {sf::Color::White};

    sf::Vector3f direction {0.0f, 0.0f, -1.0f};

    float radius{100.f};
    float intensity{1.f};
    
    float quadratic_attenuation {1.f};
    float linear_attenuation {0.0f};

    bool is_selected{false};

};