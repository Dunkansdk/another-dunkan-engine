#pragma once

#include <entitymanager.hpp>

namespace Quark {

    struct PhysicsSystem {

		public:
			void update(EntityManager<Entity>& entity_manager);

		private:
			EntityManager<Entity> entity_manager;

	};

}
