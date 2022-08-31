#include <iostream>
#include <cstdint>
#include <tuple>

namespace Quark {

    namespace cpp_function {
        /*
         * constant
         */
        template<typename T, T VALUE>
        struct constant { static constexpr T value { VALUE }; };
        struct false_type : constant<bool, false> {};
        struct true_type : constant<bool, true> {};

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

    }

    template<typename TAG_LIST> // TAGS = Typelist<TEnemy, TPlayer, TBullet>
    struct tags_traits{
        using mask_type = cpp_function::templateif_t<(TAG_LIST::size() <= 8),
            uint8_t, cpp_function::templateif_t<(TAG_LIST::size() <= 16),
            uint16_t, uint32_t>
        >;

        consteval static uint8_t size() noexcept { return TAG_LIST::size() ;}
        template<typename TAG>
        consteval static uint8_t id()   noexcept {
            static_assert(TAG_LIST::template contains<TAG>());
            return TAG_LIST::template pos<TAG>();
        }
        template<typename TAG>
        consteval static mask_type mask() noexcept { return (1 << id<TAG>()) ;}
    };

    template<typename COMPONENTS>
    struct component_traits : tags_traits<COMPONENTS> { };

    template<typename COMPONENTS, typename TAGS> // COMPONENTS = Typelist<CPhysics, CRender, CHealth>
    struct Game {
        using tags = tags_traits<TAGS>;
        using components = component_traits<COMPONENTS>;
        using storage_type = cpp_function::replace_t<std::tuple, COMPONENTS>;

        std::tuple<COMPONENTS> m_components{}; // std::tuple<CPhysics, CRender, CHealth>
    };

}
