#pragma once

#include <imgui.h>
#include <imgui-SFML.h>

struct PhysicsComponent {

    float x{}, y{}, z{};
    float velocity_x{};
    float velocity_y{};
    float velocity_z{};

    void debug() {
        ImGui::DragFloat("X", &x, .05f);
        ImGui::DragFloat("Y", &y, .05f);
        ImGui::DragFloat("Z", &z, .05f);
    }

};
