#pragma once

#include <SFML/Graphics.hpp>

struct LightComponent {
    sf::Color diffuse_color {sf::Color::White};
    sf::Color specular_color {sf::Color::White};
    float contant_attenuation {1.0f};
    float linear_attenuation {};
    float quadratic_attenuation {};
    bool cast_shadow {};
    sf::Vector3f direction {0.0f, 0.0f, -1.0f};
};