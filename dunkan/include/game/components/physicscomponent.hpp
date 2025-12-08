#pragma once

#include <glm/glm.hpp>

struct PhysicsComponent {

    float x{}, y{}, z{};
    float velocity_x{};
    float velocity_y{};
    float velocity_z{};

    glm::vec3 position() {
        return position(glm::vec2{0.0, 0.0});
    }

    glm::vec3 position(glm::vec2 shift) {
        return glm::vec3{x - shift.x, y - z - shift.y, z};
    }
    
};
