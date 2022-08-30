#include "game.hpp"
#include <type_traits>

using namespace Quark;

struct CPhysics { static constexpr uint8_t id {0}; };
struct CRender  { static constexpr uint8_t id {1}; };
struct CHealth  { static constexpr uint8_t id {2}; };
using ComponentTypes = METACPP::Typelist<CPhysics, CRender, CHealth>;

struct TPlayer  { static constexpr uint8_t id {0}; }; // 0b001 2^0 1 << 0
struct TEnemy   { static constexpr uint8_t id {1}; }; // 0b010 2^1 1 << 0
struct TBullet  { static constexpr uint8_t id {2}; }; // 0b100 2^2 1 << 0
using TagTypes = METACPP::Typelist<TPlayer, TEnemy, TBullet>;

// Macro
void seetype(auto) { std::cout << __PRETTY_FUNCTION__ << "\n"; }

template<typename T>
struct S {
    static constexpr bool value { true };
};

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

int main() {

    seetype(nth_type<0, char, int>{});
    seetype(nth_type<0, char, int, float>::type{});
    seetype(nth_type<1, char, int, float>::type{});
    seetype(nth_type<2, char, int, float>::type{});

    return 1;
}
