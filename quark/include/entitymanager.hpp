#pragma once

#include <vector>
#include <entity.hpp>
#include <slotmap.hpp>

namespace Quark {

	template <typename EntityType>
	struct EntityManager {
		using TypeProcessFunc = void (*)(EntityType&);

		EntityManager(std::size_t default_size = 100) {
    	    m_entities.reserve(default_size);
		}

		auto& create_entity() { return this->m_entities.emplace_back(); }

		void forall(TypeProcessFunc process) {
			for(auto& entity : m_entities)
				process(entity);
		}

	private:
		std::vector<EntityType> m_entities{};

	};

}
