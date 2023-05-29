#pragma once

#include "game/types.hpp"
#include <cwchar>
#include <ratio>

using RenderSystem_c = ADE::META_TYPES::Typelist<RenderComponent, PhysicsComponent>;
using RenderSystem_t = ADE::META_TYPES::Typelist<>;

struct DebugSystem {

    void update(EntityManager& entity_manager) {
        ImGui::Begin("Entities");

        entity_manager.foreach<RenderSystem_c, RenderSystem_t>
        ([&](Entity& entity, RenderComponent& render, PhysicsComponent& physics)
        {
            if (ImGui::TreeNode(std::to_string(static_cast<int>(entity.get_id())).c_str()))
            {
                if(physics.is_debug) {
                    ImGui::DragFloat("X", &physics.x, .1f);
                    ImGui::DragFloat("Y", &physics.y, .1f);
                    ImGui::DragFloat("Z", &physics.z, .01f);
                }
                ImGui::DragFloat("Height", &render.height, .5f);
                ImGui::DragFloat("Scale", &render.scale, .01f);
                ImGui::TreePop();
            }
        });

        ImGui::End();
    }
};

