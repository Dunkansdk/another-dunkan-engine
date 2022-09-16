#include <iostream>
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Clock.hpp"
#include "SFML/Window/ContextSettings.hpp"
#include "SFML/Window/Mouse.hpp"
#include "SFML/Window/WindowStyle.hpp"
#include "game/types.hpp"
#include "engine/assets/assetmanager.hpp"

void game_entities(EntityManager& entity_manager) {

    auto& entity1 = entity_manager.create_entity();
    entity_manager.add_component<PhysicsComponent>(entity1, PhysicsComponent{.x = -69.f, .y = -69.f, .z = 1.f});
    RenderComponent& render = entity_manager.add_component<RenderComponent>(entity1, RenderComponent{});
    render.set_texture("sarco-color.png");
    render.set_3D_textures("sarco-heightmap.png", "sarco-normal.png");
    entity_manager.add_component<NameComponent>(entity1, NameComponent{"qweqwe"});

}

void update(sf::RenderWindow& window) {
    sf::Clock clock;

    EntityManager entity_manager;
    RenderSystem render_system {};
    PhysicsSystem physics_system {};

    game_entities(entity_manager);

    clock.restart();
    while(window.isOpen())
    {
        sf::Time elapsed_time = clock.restart();
        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

    //    std::cout << worldPos.x << ", " << worldPos.y << std::endl;

        physics_system.update(entity_manager);
        render_system.update(entity_manager, window);
    }

    window.close();
}


int main() {

    // To engine wrapper class
    sf::RenderWindow window;
    sf::VideoMode video_mode(800, 600);

    sf::View view = window.getDefaultView();
    view.setSize(800.f, 600.f);
    view.setCenter(400.f, 300.f);

    sf::ContextSettings context_settings;
    context_settings.depthBits = 24;
    context_settings.antialiasingLevel = 2;
    context_settings.sRgbCapable = false;

    window.create(video_mode, "Window", sf::Style::Close, context_settings);
    window.setView(view);

    update(window);

}
