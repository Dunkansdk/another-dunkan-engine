#include "engine/entitymanager.hpp"
#include "game/components/physicscomponent.hpp"
#include "game/components/rendercomponent.hpp"
#include "game/components/namecomponent.hpp"

using EntityManager = ADE::EntityManager<ADE::META_TYPES::Typelist<NameComponent, PhysicsComponent, RenderComponent>>;
using Entity        = EntityManager::Entity;