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
            const char * entity_id = std::to_string(static_cast<int>(entity.get_id())).c_str();
            if (ImGui::TreeNode(entity_id))
            {
                render.debug();
                physics.debug();
                ImGui::TreePop();
            }
        });

        ImGui::End();
    }
};

