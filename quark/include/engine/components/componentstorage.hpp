#pragma once

#include <cstddef>
#include <cstdint>
#include <tuple>
#include "engine/utils/slotmap.hpp"
#include "engine/components/traits.hpp"

namespace Quark {

    template <typename Component0, typename Component1, typename Component2, std::size_t Capacity = 10>
    struct ComponentStorage {

        template<typename Component>
        [[nodiscard]] static constexpr std::size_t get_id() noexcept { return get_temp_id(Component{}); }

        template<typename Component>
        [[nodiscard]] static constexpr std::size_t get_mask() noexcept {
            return getMask(get_id<Component>());
        }

        template<typename Component>
        [[nodiscard]] constexpr auto& get_storage() noexcept {
            return std::get<get_id<Component>()>(m_components_tuple);
        }

/*
        template<typename Component>
        [[nodiscard]] constexpr auto& get_storage() noexcept {
            using store_type = Slotmap<Component, Capacity>;
            return std::get<store_type>(m_components_tuple);
        }
*/
    private:
        [[nodiscard]] static constexpr std::size_t get_temp_id(Component0) noexcept { return 0; }
        [[nodiscard]] static constexpr std::size_t get_temp_id(Component1) noexcept { return 1; }
        [[nodiscard]] static constexpr std::size_t get_temp_id(Component2) noexcept { return 2; }

        [[nodiscard]] static constexpr std::size_t getMask(std::uint8_t component_id) noexcept { return 1 << component_id; }

        std::tuple<
            Slotmap<Component0, Capacity>,
            Slotmap<Component1, Capacity>,
            Slotmap<Component2, Capacity>
        > m_components_tuple{};

    };

}
