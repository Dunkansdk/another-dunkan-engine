#pragma once

#include <imgui.h>
#include <imgui-SFML.h>

struct PhysicsComponent {

    float x{}, y{}, z{};
    float velocity_x{};
    float velocity_y{};
    float velocity_z{};
    bool is_debug{};

};
