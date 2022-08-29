#pragma once

#include "pyshicscomponent.hpp"
#include "rendercomponent.hpp"

namespace Quark {

	struct Entity {
	public:
		PhysicsComponent physics;
		RenderComponent render;
	};

}
