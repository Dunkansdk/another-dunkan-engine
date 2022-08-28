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
  MemoryViewer::show_memory_object(names);
  [[maybe_unused]] auto key = names.push_back(NameComponent{"Emanuel"});
  std::printf("Insert. Key: (%ld, %ld)\n", key.id, key.generation);
  MemoryViewer::show_memory_object(names);
  key = names.push_back(NameComponent{"Testing"});
  std::printf("Insert. Key: (%ld, %ld)\n", key.id, key.generation);
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
