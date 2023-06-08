#pragma once

#include "SFML/Window/Window.hpp"
#include "game/components/cameracomponent.hpp"
#include "game/components/physicscomponent.hpp"
#include "game/types.hpp"

using CameraSystem_c = ADE::META_TYPES::Typelist<CameraComponent, PhysicsComponent>;
using CameraSystem_t = ADE::META_TYPES::Typelist<>;

struct CameraSystem {

    void update(sf::RenderWindow& window, float delta) {

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            movement.x -= 500;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            movement.x += 500;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            movement.y -= 500;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            movement.y += 500;

        sf::View view = window.getView();
        view.move(movement * delta);
        window.setView(view);

        movement = {0.f, 0.f};
        
        // entity_manager.foreach<CameraSystem_c, CameraSystem_t>
        // ([&](Entity& entity, CameraComponent& camera, PhysicsComponent& physics)
        // {
        //     sf::View view = window.getView();
        //     view.setSize(camera.getSize());
        //     view.setCenter((view.getViewport().height * 0.5), (view.getViewport().width * 0.5));
        //     window.setView(view);
        // });
    }

private:
    sf::Vector2f movement{0.f, 0.f};  

};
