#pragma once

#include <SFML/Graphics.hpp>

struct ConfigurationComponent {

    bool enable_SSAO{true};
    bool enable_SRGB{false};

    float exposure{.5f};    

    int debug_screen{0};

    sf::Color ambient_light{sf::Color{ 124, 124, 132, 255 }};

};