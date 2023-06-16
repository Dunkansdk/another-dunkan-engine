#pragma once

#include <SFML/Graphics.hpp>

enum ShadowCastingType {
    NO_SHADOW,
    DIRECTIONNAL,
    DYNAMIC,
    ALL_SHADOWS
};

struct ShadowComponent {

    ShadowComponent() {}

    ShadowComponent(ShadowCastingType type) {
        this->shadow_type = type;
    }
    
    ShadowCastingType shadow_type{};
    
    std::map<LightComponent*, sf::Texture>  m_shadow_map{};
    std::map<LightComponent*, sf::Sprite>   m_shadow_sprite{};
    std::map<LightComponent* ,sf::IntRect>  m_shadow_max_shift{};

};  