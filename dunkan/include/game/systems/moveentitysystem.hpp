#pragma once

#include "game/types.hpp"

using MoveEntitySystem_c = ADE::META_TYPES::Typelist<RenderComponent, PhysicsComponent>;
using MoveEntitySystem_t = ADE::META_TYPES::Typelist<>;

struct SelectedEntity {
    std::size_t id;
    float distance;
};

struct MoveEntitySystem {

    void update(EntityManager& entity_manager, sf::RenderWindow& window, sf::Event& event) {

        sf::View current_view = window.getView();
        sf::Vector2f view_shift = current_view.getCenter();
        view_shift -= sf::Vector2f(current_view.getSize().x / 2, current_view.getSize().y / 2);

        float x = window.mapPixelToCoords(sf::Mouse::getPosition(window)).x + view_shift.x;
        float y = window.mapPixelToCoords(sf::Mouse::getPosition(window)).y + view_shift.y;

        if(!ImGui::GetIO().WantCaptureMouse && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {

            entity_manager.foreach<MoveEntitySystem_c, MoveEntitySystem_t>
            ([&](Entity& entity, RenderComponent& render, PhysicsComponent& physics)
            {
                if(render.moveable && selected.id == entity.get_id()) {
                    if(!render.is_selected) render.is_selected = true;

                    // Move entity
                    physics.x = x - (render.get_texture().getSize().x / 2);
                    physics.y = y - (render.get_texture().getSize().y / 2);

                    // Zoom event
                    if(event.type == sf::Event::MouseWheelScrolled) {
                        if(event.mouseWheelScroll.delta < 0) {
                            render.scale +=  0.05f;
                        } else if(event.mouseWheelScroll.delta > 0) {
                            render.scale -=  0.05f;
                        }
                    }

                } else {
                    if(render.is_selected) render.is_selected = false;
                }
                
                sf::FloatRect entity_rect;
                entity_rect.height = render.getGlobalBounds().height * 0.6f;
                entity_rect.width = render.getGlobalBounds().width * 0.6f;
                entity_rect.top = render.getGlobalBounds().top * 0.6f;
                entity_rect.left = render.getGlobalBounds().left * 0.6f;

                if (render.moveable && render.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window))))
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
        float x_center = (render.getGlobalBounds().left + render.getGlobalBounds().width / 2);
        float y_center = (render.getGlobalBounds().top + render.getGlobalBounds().height / 2) ;
        float distance = std::sqrt((x - x_center) * (x - x_center) + (y - y_center) * (y - y_center));
        std::cout << "######## Entity: " << entity.get_id() << "\n";
        std::cout << "x_center: " << (x - x_center) * (x - x_center) << "\n";
        std::cout << "y_center: " << (y - y_center) * (y - y_center) << "\n";
        std::cout << "Distance Setter: " << distance << "\n";
        return SelectedEntity{(std::size_t)entity.get_id(), distance};
    }

};
