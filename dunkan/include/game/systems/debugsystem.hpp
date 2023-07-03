#pragma once

#include "game/types.hpp"
#include <cwchar>
#include <ratio>

using DebugRenderSystem_c = ADE::META_TYPES::Typelist<RenderComponent, PhysicsComponent>;
using DebugRenderSystem_t = ADE::META_TYPES::Typelist<>;

using DebugLightSystem_c = ADE::META_TYPES::Typelist<LightComponent, PhysicsComponent>;
using DebugLightSystem_t = ADE::META_TYPES::Typelist<>;

struct DebugSystem {

    void update(EntityManager& entity_manager) {

        ImGui::Begin("Entities");

        entity_manager.foreach<DebugRenderSystem_c, DebugRenderSystem_t>
        ([&](Entity& entity, RenderComponent& render, PhysicsComponent& physics)
        {
            if (ImGui::TreeNode(std::to_string(static_cast<int>(entity.get_id())).c_str()))
            {
                ImGui::DragFloat("X", &physics.x, .1f);
                ImGui::DragFloat("Y", &physics.y, .1f);
                ImGui::DragFloat("Z", &physics.z, .01f);
                ImGui::DragFloat("Height", &render.height, .5f);
                ImGui::DragFloat("Scale", &render.scale, .01f);
                ImGui::SliderFloat("Roughness", &render.roughness, 0.0f, 1.0f);
                ImGui::SliderFloat("Metalness", &render.metalness, 0.0f, 1.0f);
                ImGui::SliderFloat("Translucency", &render.translucency, 0.0f, 1.0f);
                ImGui::TreePop();
            }
        });

        entity_manager.foreach<DebugLightSystem_c, DebugLightSystem_t>
        ([&](Entity& entity, LightComponent& light, PhysicsComponent& physics)
        {
            if (ImGui::TreeNode(std::to_string(static_cast<int>(entity.get_id())).c_str()))
            {
                if(light.light_type == LightType::DIRECTIONAL) {
                    // if (ImGui::Button("Recalculate Direction")) {
                    //     light.require_shadow_computation = true;
                    // }
                    ImGui::SliderFloat("Direction X", &light.direction.x, -2.0f, 1.0f);
                    ImGui::SliderFloat("Direction Y", &light.direction.y, -2.0f, 1.0f);
                    ImGui::SliderFloat("Direction Z", &light.direction.z, -2.0f, 0.0f);
                } else {
                    ImGui::DragFloat("X", &physics.x, 1.0f);
                    ImGui::DragFloat("Y", &physics.y, 1.0f);
                    ImGui::DragFloat("Z", &physics.z, 1.0f);
                }
                ImGui::SliderFloat("Radius", &light.radius, 0.0f, 10.f);
                ImGui::SliderFloat("Intensity", &light.intensity, 0.f, 100.f);
                
                ImGui::TreePop();
            }
        });

        ImGui::End();
    }

};

