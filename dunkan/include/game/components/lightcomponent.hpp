#pragma once

#include <SFML/Graphics.hpp>

enum LightType {
    SPOT, 
    DIRECTIONAL, 
    OMNI
};

class LightComponent {

public:

    LightComponent() {};
    LightComponent(LightType light_type, sf::Color diffuse, sf::Color specular, 
        sf::Vector3f direction, float radius, float intensity, bool cast_shadow) : 
            light_type(LightType::DIRECTIONAL), 
            diffuse_color(sf::Color::White),
            specular_color(sf::Color::White), 
            direction(sf::Vector3f(0.0f, 0.0f, -1.0f)),
            radius(50.0f),
            intensity(6.0f),
            cast_shadow(true) 
    {
        this->light_type = light_type;
        this->diffuse_color = diffuse;
        this->specular_color = specular;
        this->direction = direction;
        this->linear_attenuation = radius;
        this->quadratic_attenuation = intensity;
        this->cast_shadow = cast_shadow;
    }

    LightType light_type{LightType::DIRECTIONAL};

    sf::Color diffuse_color {sf::Color::White};
    sf::Color specular_color {sf::Color::White};

    float contant_attenuation {1.0f};
    float linear_attenuation {0.0f};
    float quadratic_attenuation {0.0f};

    sf::Vector3f direction {0.0f, 0.0f, -1.0f};

    float radius {50.0f};
    float intensity{1.0f};

    bool cast_shadow{true};

    bool require_shadow_computation() {
        return this->m_require_shadow_computation;
    }

    void require_shadow_computation(bool require) {
        this->m_require_shadow_computation = require;
    }

private:
    bool m_require_shadow_computation{false};

};