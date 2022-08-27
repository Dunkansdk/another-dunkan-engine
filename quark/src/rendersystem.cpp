#include <rendersystem.hpp>

namespace QUARK {

  void RenderSystem::update(EntityManager<Entity> entity_manager, Engine& engine)
  {
    entity_manager.forall([](Entity& entity) {
      // entity.render
    });
    engine.clear_screen();
    engine.end_scene();
  }

}
