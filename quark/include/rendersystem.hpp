#pragma once

#include "entitymanager.hpp"
#include <engine.hpp>

namespace QUARK {

	struct RenderSystem {

		public:
			void update(EntityManager<Entity> entity_manager, Engine& engine);

		private:

	};

}
