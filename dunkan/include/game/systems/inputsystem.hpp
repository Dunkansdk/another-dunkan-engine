#pragma once

#include "game/types.hpp"

using InputSystem_c = ADE::META_TYPES::Typelist<RenderComponent, PhysicsComponent>;
using InputSystem_t = ADE::META_TYPES::Typelist<>;

struct SelectedEntity {
    std::size_t id;
    float distance;
};

struct InputSystem {

    void update(EntityManager& entity_manager, sf::RenderWindow& window, sf::Event& event) {

        float x = sf::Mouse::getPosition(window).x - window.getView().getSize().x / 2;
        float y = sf::Mouse::getPosition(window).y - window.getView().getSize().y / 2;
        
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            entity_manager.foreach<InputSystem_c, InputSystem_t>
            ([&](Entity& entity, RenderComponent& render, PhysicsComponent& physics)
            {

                if(selected.id == entity.get_id()) {
                    if(!render.is_selected) render.is_selected = true;
                    physics.x = x;
                    physics.y = y;
                } else {
                    if(render.is_selected) render.is_selected = false;
                }
                
                sf::FloatRect entity_rect;
                entity_rect.height = render.getGlobalBounds().height * 0.6;
                entity_rect.width = render.getGlobalBounds().width * 0.6;
                entity_rect.top = render.getGlobalBounds().top * 0.6;
                entity_rect.left = render.getGlobalBounds().left * 0.6;

                if (render.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window))))
                {
                    if(event.type == sf::Event::MouseButtonPressed) {
                        SelectedEntity tmp_selected{get_optimal_selection(entity, render, x, y)};
                        if(selected.id == 0 || tmp_selected.distance < selected.distance) {
                            selected.distance = tmp_selected.distance;
                            selected.id = tmp_selected.id;
                        }
                    }
                }
            });
        }

        if(event.type == sf::Event::MouseButtonReleased) {
            selected = {0, 0};
            // std::cout << "########### Cleanup Selected Entity\n";
        }
    }

private:
    SelectedEntity selected{};

    SelectedEntity get_optimal_selection(Entity& entity, RenderComponent& render, float x, float y) const noexcept {
        float x_center = (render.getGlobalBounds().width  - render.getGlobalBounds().left) / 2;
        float y_center = (render.getGlobalBounds().height - render.getGlobalBounds().top) / 2;
        float distance = std::sqrt(x_center - x) + std::sqrt(y_center - y);
        // std::cout << "######## Entity: " << entity.get_id() << "\n";
        // std::cout << "x_center: " << x - x_center << "\n";
        // std::cout << "y_center: " << y - y_center << "\n";
        // std::cout << "Distance Setter: " << distance << "\n";
        return SelectedEntity{(std::size_t)entity.get_id(), distance};
    }

};
