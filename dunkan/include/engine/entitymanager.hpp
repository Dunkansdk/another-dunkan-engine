#pragma once

#include <vector>
#include "engine/components/componentstorage.hpp"

namespace ADE {

	template <typename COMPONENT_LIST, typename TAG_LIST = META_TYPES::Typelist<>, std::size_t CAPACITY = 10>
	struct EntityManager {
        struct Entity;

		using type_process_func     = void (*)(Entity&);
        using component_storage_t   = ComponentStorage<COMPONENT_LIST, TAG_LIST>;
        using components_temp       = COMPONENT_LIST;
        template <typename T>
        using to_key_type           = typename Slotmap<T, CAPACITY>::key_type;

        struct Entity {
            using key_type_list = META_TYPES::mp_transform<to_key_type, components_temp>;
            using key_storage_t = META_TYPES::replace_t<std::tuple, key_type_list>;
            
            template <typename COMPONENT>
            void add_component(to_key_type<COMPONENT> key) {
                m_component_mask |= component_storage_t::component_info::template mask<COMPONENT>();
                std::get<to_key_type<COMPONENT>>(m_component_keys) = key;
            }

            // 0101 & 0001 -> 0001
            template <typename COMPONENT>
            bool has_component() const noexcept {
                auto mask = component_storage_t::component_info::template mask<COMPONENT>();
                return m_component_mask & mask;
            }

            template <typename COMPONENT>
            to_key_type<COMPONENT> get_component_key() const {
                assert(has_component<COMPONENT>());
                return std::get<to_key_type<COMPONENT>>(m_component_keys);
            }

        private:
            std::size_t id{next_id++};
            typename component_storage_t::component_info::mask_type m_component_mask{};
            typename component_storage_t::tag_info::mask_type       m_tag_mask{};
            key_storage_t m_component_keys {};
            
            inline static std::size_t next_id{1};

        };

		EntityManager(std::size_t default_size = 100) {
    	    m_entities.reserve(default_size);
		}

        template<typename COMPONENT, typename... INITIAL_TYPES>
        COMPONENT& add_component(Entity& entity, INITIAL_TYPES&&... values) {
            auto& storage = m_components.template get_storage<COMPONENT>();
            to_key_type<COMPONENT> key;

            if(entity.template has_component<COMPONENT>()) {
                key = entity.template get_component_key<COMPONENT>();
            } else {
                key = storage.push_back(COMPONENT{std::forward<INITIAL_TYPES>(values)...});
                entity.template add_component<COMPONENT>(key);
            }

            return storage[key];
        }

        // template<typename COMPONENT>
        // COMPONENT& get_component(Entity& entity) {
        //     assert(entity.template has_component<COMPONENT>());
        //     auto& storage = m_components.template get_storage<COMPONENT>();
        //     to_key_type<COMPONENT> key = entity.template get_component_key<COMPONENT>();
        //     return storage[key];
        // }

		auto& create_entity() { return this->m_entities.emplace_back(); }

		void forall(type_process_func process) {
			for(auto& entity : m_entities)
				process(entity);
		}

	private:
		std::vector<Entity> m_entities{};
		component_storage_t m_components{};

	};

}
