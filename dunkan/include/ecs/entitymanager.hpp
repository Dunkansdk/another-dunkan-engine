#pragma once

#include <chrono>
#include <vector>
#include "ecs/components/componentstorage.hpp"
#include "ecs/utils/typelist.hpp"

namespace ADE {

    template <typename COMPONENT_LIST, typename SINGLETON_LIST, typename TAG_LIST = META_TYPES::Typelist<>, std::size_t CAPACITY = 10>
    struct EntityManager {

        struct Entity;

        template <typename T>
        using to_key_type           = typename Slotmap<T, CAPACITY>::key_type;
        using component_storage_t   = ComponentStorage<COMPONENT_LIST, SINGLETON_LIST, TAG_LIST, CAPACITY>;
        using supported_components  = COMPONENT_LIST;

        struct Entity {
            using key_type_list = META_TYPES::mp_transform<to_key_type, COMPONENT_LIST>;
            using key_storage_t = META_TYPES::replace_t<std::tuple, key_type_list>;

            template <typename COMPONENT>
            constexpr void add_component(to_key_type<COMPONENT> key) {
                m_component_mask |= component_storage_t::component_info::template mask<COMPONENT>();
                std::get<to_key_type<COMPONENT>>(m_component_keys) = key;
            }

            template <typename COMPONENT>
            [[nodiscard]] constexpr bool has_component() const noexcept {
                auto mask = component_storage_t::component_info::template mask<COMPONENT>();
                return m_component_mask & mask;
            }

            template <typename COMPONENT>
            [[nodiscard]] constexpr to_key_type<COMPONENT> get_component_key() const {
                assert(has_component<COMPONENT>());
                return std::get<to_key_type<COMPONENT>>(m_component_keys);
            }

            template <typename COMPONENT>
            void erase_component() {
                m_component_mask ^= component_storage_t::component_info::template mask<COMPONENT>();
            }

            template <typename TAG>
            constexpr void add_tag() {
                m_tag_mask |= component_storage_t::tag_info::template mask<>();
            }

            template <typename TAG>
            [[nodiscard]] constexpr bool has_tag() const noexcept {
                auto mask = component_storage_t::tag_info::template mask<TAG>();
                return m_tag_mask & mask;
            }

            bool is_alive() const noexcept {
                return this->alive;
            }

            void kill() noexcept {
                this->alive = false;
            }

            [[nodiscard]] constexpr int get_id() const noexcept {
                return static_cast<int>(this->id);
            }

        private:
            typename component_storage_t::component_info::mask_type m_component_mask{};
            typename component_storage_t::tag_info::mask_type       m_tag_mask{};
            key_storage_t m_component_keys {};

            std::size_t id{next_id++};
            bool alive{true};

            inline static std::size_t next_id{1};

        };

        /**
         * Constructor
         **/
        EntityManager(std::size_t default_size = 100) {
            m_entities.reserve(default_size);
        }

        template<typename COMPONENT, typename... INITIAL_TYPES>
        COMPONENT& add_component(Entity& entity, INITIAL_TYPES&&... values) {
            if(entity.template has_component<COMPONENT>()) {
                return get_component<COMPONENT>(entity);
            }
            // TODO: Create function create_new_component private
            auto& storage = m_components.template get_storage<COMPONENT>();
            to_key_type<COMPONENT> key = storage.push_back(COMPONENT{std::forward<INITIAL_TYPES>(values)...});
            entity.template add_component<COMPONENT>(key);
            return storage[key];
        }

        template<typename COMPONENT>
        COMPONENT& get_component(Entity const& entity) {
            assert(entity.template has_component<COMPONENT>());
            auto& storage = m_components.template get_storage<COMPONENT>();
            to_key_type<COMPONENT> key = entity.template get_component_key<COMPONENT>();
            return storage[key];
        }

        template<typename COMPONENT>
        auto const& get_singleton_component() const {
            return m_components.template get_singleton_component<COMPONENT>();
        }

        template<typename COMPONENT>
        auto& get_singleton_component() {
            return m_components.template get_singleton_component<COMPONENT>();
        }

        template<typename COMPONENT>
        bool erase_component(Entity& entity) {
            if(!entity.template has_component<COMPONENT>()) return false;
            auto& storage = m_components.template get_storage<COMPONENT>();
            to_key_type<COMPONENT> key = entity.template get_component_key<COMPONENT>();
            entity.template erase_component<COMPONENT>();
            return storage.erase(key);
        }

        void kill(Entity& entity) {
            entity.kill();
            erase_components_impl(entity, COMPONENT_LIST{});
        }

	    auto& create_entity() { return this->m_entities.emplace_back(); }

        template<typename TFunc>
        void forall(TFunc&& process) {
            for(auto& entity : m_entities) {
                process(entity);
            }
        }

        /**
         * typename C -> Typelist<Components...>
         * typename T -> Typelist<Tags...>
         */
        template<typename C, typename T>
        void foreach(auto&& process) {
            foreach_impl(process, C{}, T{});
        }

        std::size_t get_entities_count() const noexcept {
            return m_entities.size();
        }

        void refresh() {
            m_entities.erase(
                std::remove_if(m_entities.begin(), m_entities.end(),
                [](const Entity& entity) { return !entity.is_alive(); }),
                m_entities.end());
        }

    private:
        std::vector<Entity> m_entities{};
	    component_storage_t m_components{};
        std::size_t size{0}, size_next{0};

        template <typename... C, typename... T>
        void foreach_impl(auto&& process, META_TYPES::Typelist<C...>, META_TYPES::Typelist<T...>) {
            for(auto& entity : m_entities) {
                if(entity.is_alive()) {
                    auto has_components = (true && ... && entity.template has_component<C>());
                    if(has_components)
                        process(entity, get_component<C>(entity)...);
                }
            }
        }

        template <typename... C>
        bool erase_components_impl(Entity& entity, META_TYPES::Typelist<C...>) {
            return (erase_component<C>(entity),...);
        }
    };

}
