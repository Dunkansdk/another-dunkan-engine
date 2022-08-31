#include "game.hpp"
#include <type_traits>

#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

using namespace Quark;

struct CPhysics { static constexpr uint8_t id {0}; };
struct CRender  { static constexpr uint8_t id {1}; };
struct CHealth  { static constexpr uint8_t id {2}; };
using ComponentTypes = cpp_function::Typelist<CPhysics, CRender, CHealth>;

struct TPlayer  { static constexpr uint8_t id {0}; }; // 0b001 2^0 1 << 0
struct TEnemy   { static constexpr uint8_t id {1}; }; // 0b010 2^1 1 << 0
struct TBullet  { static constexpr uint8_t id {2}; }; // 0b100 2^2 1 << 0
using TagTypes = cpp_function::Typelist<TPlayer, TEnemy, TBullet>;

using GameType = Game<ComponentTypes, TagTypes>;

// Macro
void seetype(auto) { std::cout << __PRETTY_FUNCTION__ << "\n"; }

int main() {

    static_assert(TBullet::id == 2);
    static_assert(GameType::tags::size() == 3);

    static_assert(GameType::tags::id<TBullet>() == 2);
    static_assert(GameType::tags::id<TEnemy>() == 1);

    static_assert(GameType::tags::mask<TEnemy>() == 0b010);
    static_assert(GameType::tags::mask<TPlayer>() == 0b001);

    std::cout << cpp_function::pos_type<int, int, void, float>::value << "\n";
    std::cout << cpp_function::pos_type<void, int, void, float>::value << "\n";
    std::cout << cpp_function::pos_type<float, int, void, float>::value << "\n";

    return 1;
}
