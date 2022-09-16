#include <iostream>
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Clock.hpp"
#include "SFML/Window/ContextSettings.hpp"
#include "SFML/Window/Mouse.hpp"
#include "SFML/Window/WindowStyle.hpp"
#include "game/types.hpp"
#include "engine/assets/assetmanager.hpp"

PhysicsComponent& game_entities(EntityManager& entity_manager) {

    Entity& entity1 = entity_manager.create_entity();
    entity_manager.add_component<PhysicsComponent>(entity1, PhysicsComponent{.x = 100.f, .y = 100.f, .z = 0.1f });
    RenderComponent& render = entity_manager.add_component<RenderComponent>(entity1, RenderComponent{});
    render.set_texture("abbaye_color.png");
    render.set_3D_textures("abbaye_heightmap.png", "abbaye_normal.png");
    entity_manager.add_component<NameComponent>(entity1, NameComponent{"Abbaye"});

    Entity& entity2 = entity_manager.create_entity();
    PhysicsComponent& phy = entity_manager.add_component<PhysicsComponent>(entity2, PhysicsComponent{ .x = -360.f, .y = -360.f, .z = 0.1f, .velocity_x = -0.01f, .velocity_y = -0.01f });
    RenderComponent& render2 = entity_manager.add_component<RenderComponent>(entity2, RenderComponent{});
    render2.set_texture("sarco-color.png");
    render2.set_3D_textures("sarco-heightmap.png", "sarco-normal.png");
    entity_manager.add_component<NameComponent>(entity2, NameComponent{"Sarco"});

    return phy;

}

void update(sf::RenderWindow& window) {
    sf::Clock clock;

    EntityManager entity_manager;
    RenderSystem render_system {};
    PhysicsSystem physics_system {};

    PhysicsComponent sarco_pos = game_entities(entity_manager);

    clock.restart();
    while(window.isOpen())
    {
        sf::Time elapsed_time = clock.restart();
        physics_system.update(entity_manager);
        render_system.update(entity_manager, window);
    }

    window.close();
}


int main() {

    // To engine wrapper class
    sf::RenderWindow window;
    sf::VideoMode video_mode(1280, 800);

    sf::View view = window.getDefaultView();
    view.setSize(1280.f, 800.f);
    view.setCenter(740.f, 500.f);

    sf::ContextSettings context_settings;
    context_settings.depthBits = 24;
    context_settings.antialiasingLevel = 2;
    context_settings.sRgbCapable = false;

    window.create(video_mode, "Window", sf::Style::Close, context_settings);
    window.setView(view);

    update(window);

}
