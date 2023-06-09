#pragma once

#include <SFML/Graphics.hpp>

enum LightType {
    Spot, 
    Directional, 
    Omni
};

struct LightComponent {

    LightType light_type;

    sf::Color diffuse_color {sf::Color::White};
    sf::Color specular_color {sf::Color::White};

    //float contant_attenuation {1.0f};
    float linear_attenuation {0.0f};
    float quadratic_attenuation {1.0f};

    sf::Vector3f direction {0.0f, 0.0f, -1.0f};

    float radius {50.0f};
    float intensity{1.0f};

};