#include <entitymanager.hpp>
#include <entity.hpp>

using namespace QUARK;

int main() {
  EntityManager<Entity> entity_manager = {100};

  auto& e = entity_manager.create_entity();

  return 0;
}
