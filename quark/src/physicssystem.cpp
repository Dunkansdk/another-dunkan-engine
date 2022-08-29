#include <entitymanager.hpp>
#include <physicssystem.hpp>

namespace Quark {

    void PhysicsSystem::update(EntityManager<Entity>& entity_manager)
    {
        entity_manager.forall([](Entity& entity)
        {
            entity.physics.x += entity.physics.velocity_x;
            entity.physics.y += entity.physics.velocity_y;
            entity.physics.z += entity.physics.velocity_z;
        });
    }

}
