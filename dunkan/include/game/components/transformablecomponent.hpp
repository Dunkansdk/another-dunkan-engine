#pragma once

#include "SFML/System/Vector2.hpp"
#include "SFML/System/Vector3.hpp"
#include "game/types.hpp"

struct TransformableComponent {
    sf::Vector3f position{};
    sf::Vector3f velocity{};
    float size{};
    float rotation{};
};
