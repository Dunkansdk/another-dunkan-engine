#include "game/systems/physicssystem.hpp"

using PhysicsSystem_c = ADE::META_TYPES::Typelist<PhysicsComponent>;
using PhysicsSystem_t = ADE::META_TYPES::Typelist<>;

void PhysicsSystem::update(EntityManager& entity_manager)
{
    entity_manager.foreach<PhysicsSystem_c, PhysicsSystem_t>
    ([&](Entity& entity, PhysicsComponent& physics)
    {
        physics.x += physics.velocity_x;
        physics.y += physics.velocity_y;
        physics.z += physics.velocity_z;

        if(physics.x <= -90.f) {
            entity_manager.erase_entity(entity);
        }
    });
}


