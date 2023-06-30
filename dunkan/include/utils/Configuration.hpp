#pragma once

#include "game/types.hpp"

namespace Configuration {

        static ConfigurationComponent* component;

        static void load(EntityManager& entity_manager) {
            component = &entity_manager.get_singleton_component<ConfigurationComponent>();
        }

        static ConfigurationComponent* get() noexcept {
            return component;
        }

};