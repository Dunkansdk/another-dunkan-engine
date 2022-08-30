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


int main() {

    return 1;
}
