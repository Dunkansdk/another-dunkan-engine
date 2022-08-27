#pragma once

#include <iostream>
#include <vector>
#include <entity.hpp>

namespace QUARK {

	template <typename Type>
	struct EntityManager {

		using TypeProcessFunc = void (*)(Type&);

	public:
		EntityManager(std::size_t default_size = 100) {
    	entities_.reserve(default_size);
		}

		auto& create_entity() { return this->entities_.emplace_back(); }

		void forall(TypeProcessFunc process) {
			for(auto& entity : entities_)
				process(entity);
		}

	private:
		std::vector<Type> entities_{};

	};

}
