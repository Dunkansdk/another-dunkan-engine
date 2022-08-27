#include <slotmap.hpp>
#include <engine.hpp>
#include <rendersystem.hpp>
#include <physicssystem.hpp>
#include <entitymanager.hpp>

using namespace QUARK;

struct NameComponent {
  char const name[8];
};

int main() {

  constexpr Slotmap<NameComponent, 4> names;
  std::printf("Names %ld\n", names.size());
  std::printf("Names %ld\n", names.size());
  names.capacity();

  Slotmap<float, names.size()> fs;

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
