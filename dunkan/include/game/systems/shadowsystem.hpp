#pragma once

#include <SFML/OpenGL.hpp>

#include "ecs/utils/Mathematics.hpp"
#include "sfml/TextureModifier.hpp"
#include "game/types.hpp"

using ShadowSystem_c = ADE::META_TYPES::Typelist<RenderComponent, PhysicsComponent, ShadowComponent>;
using ShadowSystem_t = ADE::META_TYPES::Typelist<>;

struct ShadowSystem {

    void calculate(EntityManager& entity_manager, LightComponent& light, sf::Vector2f view_shift) {

        entity_manager.foreach<ShadowSystem_c, ShadowSystem_t>
        ([&](Entity&, RenderComponent& render, PhysicsComponent&, ShadowComponent& shadow)
        {
            
        });
    }

    void update_shadow(EntityManager& entity_manager, LightComponent& light, sf::Vector2f view_shift) {

        entity_manager.foreach<ShadowSystem_c, ShadowSystem_t>
        ([&](Entity&, RenderComponent&, PhysicsComponent&, ShadowComponent& shadow)
        {

        });

    }

    void render(EntityManager& entity_manager, const sf::View& view, const sf::Vector2u& screen_size, LightComponent& light, sf::Shader* depth_shader) {

    }

private:
    sf::IntRect m_shadow_max_shift{};

};