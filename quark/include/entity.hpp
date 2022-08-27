#pragma once

#include "pyshicscomponent.hpp"
#include "rendercomponent.hpp"

namespace QUARK {

	struct Entity {
	public:
		Entity();
		~Entity();

		PhysicsComponent physics;
		RenderComponent render;
	};

}
