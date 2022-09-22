#pragma once

#include "game/types.hpp"

using PhysicsSystem_c = ADE::META_TYPES::Typelist<PhysicsComponent>;
using PhysicsSystem_t = ADE::META_TYPES::Typelist<>;

struct PhysicsSystem {

    void update(EntityManager& entity_manager, float delta) {


        entity_manager.foreach<PhysicsSystem_c, PhysicsSystem_t>
        ([&](Entity& entity, PhysicsComponent& physics)
        {
            physics.x += physics.velocity_x * delta;
            physics.y += physics.velocity_y * delta;
            physics.z += physics.velocity_z * delta;
        });

    }

};


