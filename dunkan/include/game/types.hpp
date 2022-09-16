#include "engine/entitymanager.hpp"
#include "engine/utils/typelist.hpp"
#include "game/components/physicscomponent.hpp"
#include "game/components/rendercomponent.hpp"
#include "game/components/namecomponent.hpp"
#include "game/systems/rendersystem.hpp"
#include "game/systems/physicssystem.hpp"

using Components    = ADE::META_TYPES::Typelist<NameComponent, PhysicsComponent, RenderComponent>;
using Tags          = ADE::META_TYPES::Typelist<>;
using EntityManager = ADE::EntityManager<Components, Tags, 1024>;
using Entity        = EntityManager::Entity;
