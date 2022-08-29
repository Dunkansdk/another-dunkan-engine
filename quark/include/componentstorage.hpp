#pragma once

#include <cstddef>
#include <cstdint>
#include <slotmap.hpp>
#include <tuple>

namespace Quark {

    template <typename Component0, typename Component1, typename Component2, std::size_t Capacity = 10>
    struct ComponentStorage {

        template<typename Component>
        [[nodiscard]] constexpr std::size_t get_id() const noexcept { return get_temp_id(Component{}); }

        template<typename Component>
        [[nodiscard]] constexpr std::size_t get_mask() const noexcept {
            return getMask(get_id<Component>());
        }

        template<typename Component>
            [[nodiscard]] constexpr auto& get_storage() const noexcept {
                using store_type = Slotmap<Component, Capacity>;
                return std::get<store_type>(m_components_tuple);
            }

    private:
        [[nodiscard]] constexpr std::size_t get_temp_id(Component0) const noexcept { return 0; }
        [[nodiscard]] constexpr std::size_t get_temp_id(Component1) const noexcept { return 1; }
        [[nodiscard]] constexpr std::size_t get_temp_id(Component2) const noexcept { return 2; }

        [[nodiscard]] constexpr std::size_t getMask(std::uint8_t component_id) const noexcept { return 1 << component_id; }

        std::tuple<
            Slotmap<Component0, Capacity>,
            Slotmap<Component1, Capacity>,
            Slotmap<Component2, Capacity>
        > m_components_tuple{};

    };

}
