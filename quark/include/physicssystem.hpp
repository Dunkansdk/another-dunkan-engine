#pragma once

#include <entitymanager.hpp>

namespace QUARK {

	struct PhysicsSystem {

		public:
			PhysicsSystem();
			~PhysicsSystem();

			void update(EntityManager<Entity>& entity_manager);

		private:
			EntityManager<Entity> entity_manager;

	};

}
