#include "engine/entitymanager.hpp"

struct EntityOld{};

struct RenderComponent {} ;

struct NameComponent {
    char name[8] {"noname"};
};

struct HealthComponent {
    int health;
    int shield;
};

using EntityManager = ADE::EntityManager<NameComponent, HealthComponent, RenderComponent>;
using Entity        = EntityManager::Entity;