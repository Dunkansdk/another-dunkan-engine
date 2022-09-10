#include "game/systems/rendersystem.hpp"

using RenderSystem_c = ADE::META_TYPES::Typelist<RenderComponent, PhysicsComponent>;
using RenderSystem_t = ADE::META_TYPES::Typelist<>;

void RenderSystem::update(EntityManager& entity_manager) {
    entity_manager.foreach<RenderSystem_c, RenderSystem_t>
    ([](auto& entity, RenderComponent& render, PhysicsComponent& physics)
    {
        std::cout << physics.x << ", " << physics.y << "\n";
    });
}
