#pragma once

#include <SFML/Graphics.hpp>

struct ConfigurationComponent {

    bool enable_SSAO{true};
    bool enable_SRGB{false};

    int debug_screen{0};

    sf::Color ambient_light{sf::Color{ 64, 64, 96 }};
    sf::Color ambient_light_srgb{sf::Color{ 32, 32, 48 }};

};