#pragma once

#include <cstddef>
#include <cstdint>
#include <tuple>
#include "engine/utils/slotmap.hpp"
#include "engine/utils/typelist.hpp"
#include "engine/components/traits.hpp"

namespace Quark {

    template <typename COMPONENT_LIST, typename TAG_LIST = cpp_function::Typelist<>, std::size_t Capacity = 10>
    struct ComponentStorage {
        using tag_info = tag_traits<TAG_LIST>;
        using component_info = component_traits<COMPONENT_LIST>;
        template <typename T> using to_tuple = cpp_function::replace_t<std::tuple, T>;
        template <typename T> using to_slotmap = Slotmap<T, Capacity>;
        using storage_type = to_tuple<cpp_function::mp_transform<to_slotmap, COMPONENT_LIST>>;

        static_assert(std::is_same_v<storage_type, void>);

        template<typename COMPONENT>
        [[nodiscard]] constexpr auto& get_storage() noexcept {
            constexpr auto id { component_info::template id<COMPONENT>() };
            return std::get<id>(m_components_tuple);
        }

    private:
        storage_type m_components_tuple{};
        // std::tuple<
        //     Slotmap<Component0, Capacity>,
        //     Slotmap<Component1, Capacity>,
        //     Slotmap<Component2, Capacity>
        // > m_components_tuple{};

    };

}
