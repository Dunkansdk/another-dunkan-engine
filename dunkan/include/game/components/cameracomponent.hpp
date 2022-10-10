#pragma once

#include "SFML/System/Vector2.hpp"

struct CameraComponent {

    float zoom{};
    sf::Vector2f size{};
    sf::Vector2f position{};

    sf::Vector2f getSize() {
        return size * zoom;
    }

};

