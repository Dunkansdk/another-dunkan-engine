#include "ecs/entitymanager.hpp"
#include "ecs/utils/typelist.hpp"
#include "game/components/physicscomponent.hpp"
#include "game/components/rendercomponent.hpp"
#include "game/components/cameracomponent.hpp"
#include "game/components/lightcomponent.hpp"
#include "sfml/AssetManager.hpp"
#include <imgui.h>
#include <imgui-SFML.h>

using Components            = ADE::META_TYPES::Typelist<LightComponent, PhysicsComponent, RenderComponent>;
using SingletonComponents   = ADE::META_TYPES::Typelist<CameraComponent>;
using Tags                  = ADE::META_TYPES::Typelist<>;
using EntityManager         = ADE::EntityManager<Components, SingletonComponents, Tags, 1024>;
using Entity                = EntityManager::Entity;
using TextureManager        = AssetManager<sf::Texture>;
