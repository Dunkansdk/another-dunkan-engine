#pragma once

#include <SFML/Graphics.hpp>

struct ConfigurationComponent {

    bool enable_SSAO{true};
    bool enable_SRGB{false};

    float exposure{.8f};    

    int debug_screen{0};

    sf::Color ambient_light{sf::Color{ 24, 24, 32, 255 }};

};