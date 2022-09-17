#pragma once

#include "SFML/Window/Window.hpp"
#include "game/components/cameracomponent.hpp"
#include "game/components/physicscomponent.hpp"
#include "game/types.hpp"

using CameraSystem_c = ADE::META_TYPES::Typelist<CameraComponent, PhysicsComponent>;
using CameraSystem_t = ADE::META_TYPES::Typelist<>;

struct CameraSystem {

    void update(EntityManager& entity_manager, sf::RenderWindow& window) {
        entity_manager.foreach<CameraSystem_c, CameraSystem_t>
        ([&](Entity& entity, CameraComponent& camera, PhysicsComponent& physics)
        {
            sf::View view = window.getView();
            view.setSize(camera.getSize());
            view.setCenter((view.getViewport().height * 0.5) - physics.x, (view.getViewport().width * 0.5) - physics.y);
            window.setView(view);
        });
    }

};
