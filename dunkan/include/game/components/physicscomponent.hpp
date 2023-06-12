#pragma once

#include <imgui.h>
#include <imgui-SFML.h>

struct PhysicsComponent {

    float x{}, y{}, z{};
    float velocity_x{};
    float velocity_y{};
    float velocity_z{};

    sf::Vector3f position() {
        return position(sf::Vector2f{0.0, 0.0});
    }

    sf::Vector3f position(sf::Vector2f shift) {
        return sf::Vector3f{x - shift.x, y - z - shift.y, z};
    }
    
};
