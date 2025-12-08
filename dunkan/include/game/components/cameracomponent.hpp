#pragma once

#include <glm/glm.hpp>

struct CameraComponent {
    glm::mat4 view{1.0f};
    glm::mat4 projection{1.0f};
    glm::vec2 position{0.0f, 0.0f};
    float zoom{1.0f};
};