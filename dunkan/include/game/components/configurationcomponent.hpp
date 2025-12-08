#pragma once

#include "vulkan/VulkanTypes.hpp"

struct ConfigurationComponent {

    bool enable_SSAO{true};
    bool enable_SRGB{false};

    float exposure{.5f};    

    int debug_screen{0};

    Color4 ambient_light{Color4(124, 124, 132, 255)};

};