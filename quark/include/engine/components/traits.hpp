#pragma once

#include "engine/utils/typelist.hpp"

namespace Quark {

    /**
     * Selects the smallest unsigned type for a mask to have enough space
     * to represent all posible types in the Typelist, up to 64 types.
     * 
     * @tparam LIST Typelist<typename... TYPES>
     */
    template<typename LIST>
    using select_smallest_mask_type_t =
        cpp_function::templateif_t<(LIST::size() <= 8), uint8_t,
        cpp_function::templateif_t<(LIST::size() <= 16), uint16_t,
        cpp_function::templateif_t<(LIST::size() <= 32), uint32_t,
        uint64_t>
        >
    >;

    /**
     * Introduces information from the tags/components of the given typelist
     * an id() and a mask() for each tag/component.
     * 
     * @tparam LIST Typelist<typename... TYPES>
     */
    template<typename LIST>
    struct common_traits {

        static_assert(LIST::size() <= 64, "ERROR: Maximum of 64 types");

        using mask_type = select_smallest_mask_type_t<LIST>;

        consteval static uint8_t size() noexcept { return LIST::size() ;}
        template<typename ITEM>
        consteval static uint8_t id()   noexcept {
            static_assert(LIST::template contains<ITEM>());
            return LIST::template pos<ITEM>();
        }
        template<typename... ITEMS>
        consteval static mask_type mask() noexcept { 
            return (0 | ... | (1 << id<ITEMS>())) ;
        }
    };

    /**
     * Specific traits for components and tags, derived from common template
     * 
     * @tparam TAGS / COMPONENTS Typelist<typename... TYPES>
     */
    template<typename TAGS>
    struct tag_traits : common_traits<TAGS> { };
    template<typename COMPONENTS>
    struct component_traits : tag_traits<COMPONENTS> { };

}