#include "game/systems/rendersystem.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"

using RenderSystem_c = ADE::META_TYPES::Typelist<RenderComponent, PhysicsComponent>;
using RenderSystem_t = ADE::META_TYPES::Typelist<>;

void RenderSystem::update(EntityManager& entity_manager, /*test*/sf::RenderWindow& window) {
    entity_manager.foreach<RenderSystem_c, RenderSystem_t>
    ([&](auto& entity, RenderComponent& render, PhysicsComponent& physics)
    {
        render.circle.setOrigin(physics.x, physics.y);
        window.draw(render.circle);
    });
}
