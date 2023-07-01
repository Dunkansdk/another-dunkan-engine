#pragma once

#include "game/types.hpp"

using MoveEntitySystem_c = ADE::META_TYPES::Typelist<RenderComponent, PhysicsComponent>;
using MoveEntitySystem_t = ADE::META_TYPES::Typelist<>;

using LightMoveEntitySystem_c = ADE::META_TYPES::Typelist<LightComponent, PhysicsComponent>;
using LightMoveEntitySystem_t = ADE::META_TYPES::Typelist<>;

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

            entity_manager.foreach<LightMoveEntitySystem_c, LightMoveEntitySystem_t>
            ([&](Entity& entity, LightComponent& light, PhysicsComponent& physics)
            {
                if(selected.id == entity.get_id()) {
                    if(!light.is_selected) light.is_selected = true;
                    // Move entity
                    physics.x = x;
                    physics.y = y;

                    // Zoom event
                    if(event.type == sf::Event::MouseWheelScrolled) {
                        if(event.mouseWheelScroll.delta < 0) {
                            physics.z +=  5.0f;
                        } else if(event.mouseWheelScroll.delta > 0) {
                            physics.z -=  5.0f;
                        }
                    }
                    if(!light.is_selected) light.is_selected = false;
                }

                if(light.light_type != LightType::DIRECTIONAL) {

                    sf::FloatRect entity_rect;
                    entity_rect.height = light.radius * 3.f;
                    entity_rect.width = light.radius * 3.f;
                    entity_rect.top = physics.x - (light.radius * 1.5f);
                    entity_rect.left = physics.y - (light.radius * 1.5f);

                    if(event.type == sf::Event::MouseButtonPressed) {
                        SelectedEntity tmp_selected{
                            get_optimal_selection(entity, 
                                entity_rect.left + entity_rect.width / 2, 
                                entity_rect.top + entity_rect.height / 2,
                                x, y)
                        };
                        if(selected.id == 0 || tmp_selected.distance < selected.distance) {
                            selected.distance = tmp_selected.distance;
                            selected.id = tmp_selected.id;
                        }
                    }
                }
            });

            entity_manager.foreach<MoveEntitySystem_c, MoveEntitySystem_t>
            ([&](Entity& entity, RenderComponent& render, PhysicsComponent& physics)
            {
                if(render.moveable && selected.id == entity.get_id()) {
                    if(!render.is_selected) render.is_selected = true;
                    // Move entity
                    physics.x = x - (render.m_texture->getSize().x / 2);
                    physics.y = y - (render.m_texture->getSize().y / 2) + physics.z;

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
                entity_rect.height = render.getGlobalBounds().height;
                entity_rect.width = render.getGlobalBounds().width;
                entity_rect.top = render.getGlobalBounds().top;
                entity_rect.left = render.getGlobalBounds().left;

                if (render.moveable && entity_rect.contains(window.mapPixelToCoords(sf::Mouse::getPosition(window))))
                {
                    if(event.type == sf::Event::MouseButtonPressed) {
                        SelectedEntity tmp_selected{
                            get_optimal_selection(entity, 
                                render.getGlobalBounds().left + render.getGlobalBounds().width / 2, 
                                render.getGlobalBounds().top + render.getGlobalBounds().height / 2,
                                x, y)
                        };
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
        }
    }

private:
    SelectedEntity selected{};

    SelectedEntity get_optimal_selection(Entity& entity, float x_center, float y_center, float x, float y) const noexcept {
        float distance = std::sqrt((x - x_center) * (x - x_center) + (y - y_center) * (y - y_center));
        return SelectedEntity{(std::size_t)entity.get_id(), distance};
    }

};
