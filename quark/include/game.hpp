#include <iostream>
#include <cstdint>

namespace Quark {

    namespace METACPP {
        /*
         * constant
         */
        template<typename T, T VALUE>
        struct constant { static constexpr T value { VALUE }; };
        struct false_type : constant<bool, false> {};
        struct true_type : constant<bool, true> {};

        /*
         * is_same_v
         */
        template<typename T, typename U>
        struct is_same : false_type {};
        template<typename T>
        struct is_same<T, T> : true_type {};
        template<typename T, typename U>
        constexpr bool is_same_v = is_same<T, U>::value;

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

        template<typename... TYPES> // Types = TEnemy, TPlayer, TBullet
        struct Typelist {
            consteval static std::size_t size() noexcept { return sizeof...(TYPES); }

            template<typename T>
            consteval static bool contains() noexcept {
                // Unfold expression -> (false || is_same_v<T, TYPES> || is_same_v<T, TYPES> || is_same_v<T, TYPES>)
                // Repeat is_same_v for each ...TYPES
                return (false || ... || is_same_v<T, TYPES>);
            }

            template<typename T>
                consteval static std::size_t pos() noexcept {
                    static_assert(contains<T>());
                    /*for(std::size_t i{}; i < size() - 1; ++i) {
                        using TypeName = nth_type_t<i, TYPES...>;
                        if(is_same_v<T, TypeName>)
                            return i;
                    }*/
                    return size() - 1;
                }
        };
    }

    template<typename TAG_LIST> // TAGS = Typelist<TEnemy, TPlayer, TBullet>
    struct tags_traits{
        consteval static std::size_t size() noexcept { return TAG_LIST::size() ;}

        template<typename TAG>
        consteval static std::size_t id()   noexcept {
            static_assert(TAG_LIST::template contains<TAG>());
            return TAG_LIST::template pos<TAG>();
        }

        template<typename TAG>
        consteval static std::size_t mask() noexcept { return (1 << id<TAG>()) ;}
    };

    template<typename COMPONENTS>
    struct component_traits : tags_traits<COMPONENTS> { };

    template<typename COMPONENTS, typename TAGS>
    struct Game {
        using tags = tags_traits<TAGS>;
        using components = component_traits<COMPONENTS>;
    };

}
