#include <slotmap.hpp>
#include <memoryviewer.hpp>
#include <engine.hpp>
#include <rendersystem.hpp>
#include <physicssystem.hpp>
#include <entitymanager.hpp>

using namespace QUARK;

struct NameComponent {
  char name[8] { "noname" };
};

int main() {

    Slotmap<NameComponent, 4> names;
    using key_type = Slotmap<NameComponent, 4>::key_type;
    std::array<key_type, 4> keys;

    MemoryViewer::show_memory_object(names);
    keys[0] = names.push_back(NameComponent{"ImFirst"});
    std::printf("Insert. Key: (%d, %d)\n", keys[0].id, keys[0].generation);
    MemoryViewer::show_memory_object(names);

    keys[1] = names.push_back(NameComponent{"ImSecnd"});
    std::printf("Insert. Key: (%d, %d)\n", keys[1].id, keys[1].generation);
    MemoryViewer::show_memory_object(names);

    keys[2] = names.push_back(NameComponent{"ImThird"});
    std::printf("Insert. Key: (%d, %d)\n", keys[2].id, keys[2].generation);
    MemoryViewer::show_memory_object(names);

    names.erase(keys[0]);
    std::printf("Erase. Key: (%d, %d)\n", keys[0].id, keys[0].generation);
    MemoryViewer::show_memory_object(names);

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
