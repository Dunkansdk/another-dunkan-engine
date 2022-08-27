#include "engine.hpp"
#include <physicssystem.hpp>
#include <entitymanager.hpp>
#include <entity.hpp>

using namespace QUARK;

int main() {

  Engine engine = { "Quark engine" };
  engine.init();
  EntityManager<Entity> entity_manager = {100};
  PhysicsSystem physics_system;

  auto& e = entity_manager.create_entity();

  while(engine.run())
  {
    physics_system.update(entity_manager);
  }

  return 1;
}
