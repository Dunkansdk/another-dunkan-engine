#pragma once

#include <imgui.h>

#include "ecs/entitymanager.hpp"
#include "ecs/utils/typelist.hpp"

#include "game/components/physicscomponent.hpp"
#include "game/components/rendercomponent.hpp"
#include "game/components/cameracomponent.hpp"
#include "game/components/lightcomponent.hpp"
#include "game/components/shadowcomponent.hpp"
#include "game/components/configurationcomponent.hpp"

#include "vulkan/VulkanResourceManager.hpp"

using Components            = ADE::META_TYPES::Typelist<LightComponent, PhysicsComponent, RenderComponent, ShadowComponent>;
using SingletonComponents   = ADE::META_TYPES::Typelist<CameraComponent, ConfigurationComponent>;
using Tags                  = ADE::META_TYPES::Typelist<>;
using EntityManager         = ADE::EntityManager<Components, SingletonComponents, Tags, 1024>;
using Entity                = EntityManager::Entity;
using ResourceManager       = VulkanResourceManager;
