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

template<typename T, T VALUE>
struct constant { static constexpr T value { VALUE }; };

template<typename T, typename... TYPES>
struct pos_type;
template<typename T, typename... TYPES>
struct pos_type<T, T, TYPES...> {
    constexpr static std::size_t value { 0 };
};
template<typename T, typename U, typename... TYPES>
struct pos_type<T, U, TYPES...> {
    constexpr static std::size_t value { 1 + pos_type<T, TYPES...>::value };
};

int main() {

    std::cout << pos_type<int, int, void, float>::value << "\n";
    std::cout << pos_type<void, int, void, float>::value << "\n";
    std::cout << pos_type<float, int, void, float>::value << "\n";

    return 1;
}
