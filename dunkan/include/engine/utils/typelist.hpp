#pragma once

#include <cstdint>
#include <type_traits>

namespace ADE {

    namespace META_TYPES {
        /*
         * constant
         */
        template<typename T, T VALUE>
        struct constant { static constexpr T value { VALUE }; };

        /*
         * nth_type
         */
        template<typename T>
        struct type_id { using type = T; };
        template<std::size_t N, typename... TYPES>
        struct nth_type { static_assert(sizeof...(TYPES) != 0, "ERROR: Typelist with 0 types."); };
        template<std::size_t N, typename... TYPES>
        using nth_type_t = typename nth_type<N, TYPES...>::type;
        template<typename T, typename... TYPES>
        struct nth_type<0, T, TYPES...> : type_id<T> {};
        template<std::size_t N, typename T, typename... TYPES>
        struct nth_type<N, T, TYPES...> : type_id<nth_type_t<N - 1, TYPES...>> {};

        /*
         * pos_type_v
         */
        template<typename T, typename... TYPES>
        struct pos_type { static_assert(sizeof...(TYPES) != 0); };
        template<typename T, typename... TYPES>
        constexpr std::size_t pos_type_v = pos_type<T, TYPES...>::value;
        template<typename T, typename... TYPES>
        struct pos_type<T, T, TYPES...> : constant<std::size_t, 0> {};
        template<typename T, typename U, typename... TYPES>
        struct pos_type<T, U, TYPES...> : constant<std::size_t, 1 + pos_type_v<T, TYPES...>> {};

        /*
         * templateif_t
         */
        template<bool CONDITION, typename T, typename F>
        struct IFT : type_id<F> {};
        template<typename T, typename F>
        struct IFT<true, T, F> : type_id<T> {};
        template<bool CONDITION, typename T, typename F>
        using templateif_t = typename IFT<CONDITION, T, F>::type;

        template<typename... TYPES> // Types = TEnemy, TPlayer, TBullet
        struct Typelist {
            consteval static std::size_t size() noexcept { return sizeof...(TYPES); }

            template<typename T>
            consteval static bool contains() noexcept {
                // Unfold expression -> (false || is_same_v<T, TYPES> || is_same_v<T, TYPES> || is_same_v<T, TYPES>)
                // Repeat is_same_v for each ...TYPES
                return (false || ... || std::is_same_v<T, TYPES>);
            }

            template<typename T>
                consteval static std::size_t pos() noexcept {
                    static_assert(contains<T>());
                    return pos_type_v<T, TYPES...>;
                }
        };

        /*
         * replace
         */
        template<template <typename...> class N, typename L>
        struct replace {};
        template<template <typename...> class N, typename... TYPES>
        struct replace<N, Typelist<TYPES...>> : type_id<N<TYPES...>> {};
        template<template <typename...> class N, typename L>
        using replace_t = typename replace<N, L>::type;
        
        template<template<class...> class F, class L> struct mp_transform_impl;
        template<template<class...> class F, template<class...> class L, class... T>
        struct mp_transform_impl<F, L<T...>> : type_id<L<F<T>...>> {};
        template<template<class...> class F, class L>
        using mp_transform = typename mp_transform_impl<F, L>::type;

    } // namespace cppfunction

} // namespace Quark