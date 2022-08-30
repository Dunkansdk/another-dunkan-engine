#include <game.hpp>

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

int main() {

    using GameType = Game<ComponentTypes, TagTypes>;
    GameType game;

    seetype(game);

    static_assert(METACPP::is_same_v<int, void> == false);
    static_assert(METACPP::is_same_v<int, int> == true);

    static_assert(TBullet::id == 2);
    static_assert(GameType::tags::size() == 3);
    static_assert(GameType::tags::id<TBullet>() == 1);
    static_assert(GameType::tags::mask<TEnemy>() == 0b010);
    static_assert(GameType::tags::mask<TBullet>() == 0b010);

    // Failed Component into TAG_LIST
    //static_assert(GameType::tags::id<CPhysics>() == 1);

    static_assert(GameType::components::size() == 3);
    static_assert(GameType::components::id<CRender>() == 1);
    static_assert(GameType::components::mask<CPhysics>() == 0b010);
    static_assert(GameType::components::mask<CRender>() == 0b010);

    return 1;
}
