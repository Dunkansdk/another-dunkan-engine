#include "engine/entitymanager.hpp"

struct EntityOld{};

struct RenderComponent {} ;

struct NameComponent {
    char name[8] {"noname"};
};

struct HealthComponent {
    std::size_t health{10};
};

using EntityManager = Quark::EntityManager<HealthComponent, NameComponent, RenderComponent>;
using Entity        = EntityManager::Entity;