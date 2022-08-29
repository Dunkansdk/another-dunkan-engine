#include "componentstorage.hpp"
#include "pyshicscomponent.hpp"
#include "rendercomponent.hpp"
#include <slotmap.hpp>
#include <memoryviewer.hpp>
#include <engine.hpp>
#include <rendersystem.hpp>
#include <physicssystem.hpp>
#include <entitymanager.hpp>

using namespace Quark;

struct NameComponent {
  char name[8] { "noname" };
};

struct HealthComponent {
    std::size_t health{10};
};

int main() {

    using ComponentStorageType = ComponentStorage<PhysicsComponent, NameComponent, HealthComponent>;

    ComponentStorageType component_storage;
    std::cout << component_storage.get_mask<HealthComponent>() << "\n";
    std::cout << component_storage.get_mask<NameComponent>() << "\n";
    std::cout << component_storage.get_mask<PhysicsComponent>() << "\n";

/*
    Engine engine = { "Quark engine" };
    engine.init();
    EntityManager<Entity> entity_manager = {100};
    PhysicsSystem physics_system;
    RenderSystem render_system;

    auto& entity_example  = entity_manager.create_entity();
    entity_example.physics = {10.f, 10.f, 1.f, 0.f};

    while(engine.run())
    {
        physics_system.update(entity_manager);
        render_system.update(entity_manager, engine);
    }
*/
    return 1;
}
