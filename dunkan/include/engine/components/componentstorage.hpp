#pragma once

#include <cstddef>
#include <cstdint>
#include <tuple>
#include "engine/utils/slotmap.hpp"
#include "engine/utils/typelist.hpp"
#include "engine/components/traits.hpp"

namespace ADE {

    template <typename COMPONENT_LIST, typename SINGLETON_LIST, typename TAG_LIST, std::size_t Capacity = 10>
    struct ComponentStorage {

        // TODO: is this necessary?
        explicit ComponentStorage() = default;
        ComponentStorage(ComponentStorage&) = delete;
        ComponentStorage(ComponentStorage&&) = delete;
        ComponentStorage& operator=(ComponentStorage&) = delete;
        ComponentStorage& operator=(ComponentStorage&&) = delete;

        using tag_info = tag_traits<TAG_LIST>;
        using component_info = component_traits<COMPONENT_LIST>;
        template <typename T> using to_tuple = META_TYPES::replace_t<std::tuple, T>;
        template <typename T> using to_slotmap = Slotmap<T, Capacity>;
        using storage_type = to_tuple<META_TYPES::mp_transform<to_slotmap, COMPONENT_LIST>>;
	using storage_singleton_type = to_tuple<SINGLETON_LIST>;

        template<typename COMPONENT>
        [[nodiscard]] constexpr auto& get_storage() noexcept {
            constexpr auto id { component_info::template id<COMPONENT>() };
            return std::get<id>(m_component_tuple);
        }

        template<typename COMPONENT>
        [[nodiscard]] constexpr auto& get_singleton_storage() noexcept {
            return std::get<COMPONENT>(m_singleton_component_tuple);
        }

        template<typename COMPONENT>
        [[nodiscard]] constexpr auto const& get_singleton_storage() const noexcept {
            return std::get<COMPONENT>(m_singleton_component_tuple);
        }

    private:
        storage_type m_component_tuple{};
	storage_singleton_type m_singleton_component_tuple{};

    };

}
