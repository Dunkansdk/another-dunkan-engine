#pragma once

#include "game/types.hpp"
#include <cwchar>
#include <ratio>

using RenderSystem_c = ADE::META_TYPES::Typelist<RenderComponent, PhysicsComponent>;
using RenderSystem_t = ADE::META_TYPES::Typelist<>;

using LightSystem_c = ADE::META_TYPES::Typelist<LightComponent, PhysicsComponent>;
using LightSystem_t = ADE::META_TYPES::Typelist<>;

struct DebugSystem {

    void update(EntityManager& entity_manager) {

        ImGui::Begin("Entities");

        entity_manager.foreach<RenderSystem_c, RenderSystem_t>
        ([&](Entity& entity, RenderComponent& render, PhysicsComponent& physics)
        {
            if (ImGui::TreeNode(std::to_string(static_cast<int>(entity.get_id())).c_str()))
            {
                ImGui::DragFloat("X", &physics.x, .1f);
                ImGui::DragFloat("Y", &physics.y, .1f);
                ImGui::DragFloat("Z", &physics.z, .01f);
                ImGui::DragFloat("Height", &render.height, .5f);
                ImGui::DragFloat("Scale", &render.scale, .01f);
                ImGui::TreePop();
            }
        });

        ImGui::End();

        ImGui::Begin("Entities");

        entity_manager.foreach<LightSystem_c, LightSystem_t>
        ([&](Entity& entity, LightComponent& light, PhysicsComponent& physics)
        {
            if (ImGui::TreeNode(std::to_string(static_cast<int>(entity.get_id())).c_str()))
            {
                ImGui::DragFloat("X", &physics.x, 1.0f);
                ImGui::DragFloat("Y", &physics.y, 1.0f);
                ImGui::DragFloat("Z", &physics.z, 1.0f);
                ImGui::DragFloat("directionx", &light.direction.x, .01f);
                ImGui::DragFloat("directiony", &light.direction.y, .01f);
                ImGui::DragFloat("directionz", &light.direction.z, .01f);
                ImGui::DragFloat("intensity", &light.intensity, .01f);
                ImGui::DragFloat("radius", &light.radius, .01f);
                
                ImGui::TreePop();
            }
        });

        ImGui::End();
    }
};

