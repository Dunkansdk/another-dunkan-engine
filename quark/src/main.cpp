#include <slotmap.hpp>
#include <memoryviewer.hpp>
#include <engine.hpp>
#include <rendersystem.hpp>
#include <physicssystem.hpp>
#include <entitymanager.hpp>

using namespace QUARK;

struct NameComponent {
  char const name[8];
};

int main() {

  std::vector example_array { 1, 2, 4, 8, 16, 32 };

  // Stack memory
  MemoryViewer::show_memory_object(example_array);

  // Heap memory
  MemoryViewer::show_memory_ptr(&example_array[0], 24);

  example_array.push_back(64);
  MemoryViewer::show_memory_object(example_array);
  MemoryViewer::show_memory_ptr(&example_array[0], 24);

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
