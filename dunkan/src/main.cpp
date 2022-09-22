#include <iostream>
#include <chrono>
#include <SFML/Graphics.hpp>
#include "SFML/Window/ContextSettings.hpp"
#include "game/systems/camerasystem.hpp"
#include "game/types.hpp"
#include "game/systems/physicssystem.hpp"
#include "game/systems/rendersystem.hpp"
#include "game/imguiconfig.hpp"

void game_entities(EntityManager& entity_manager) {

    Entity& entity1 = entity_manager.create_entity();
    entity_manager.add_component<PhysicsComponent>(entity1, PhysicsComponent{.x = -350.f, .y = 10.f, .z = 0.5f });
    RenderComponent& render = entity_manager.add_component<RenderComponent>(entity1, RenderComponent{});
    render.set_texture("abbaye_color.png");
    render.set_3D_textures("abbaye_heightmap.png", "abbaye_normal.png");

    Entity& entity2 = entity_manager.create_entity();
    entity_manager.add_component<PhysicsComponent>(entity2, PhysicsComponent{ .x = 0.f, .y = 0.f, .z = 0.5f, .velocity_x = -20.f, .velocity_y = -20.f });
    entity_manager.add_component<CameraComponent>(entity2, CameraComponent{.zoom = 1.f, .size = sf::Vector2f(1280,800)});
    RenderComponent& render2 = entity_manager.add_component<RenderComponent>(entity2, RenderComponent{});
    render2.set_texture("sarco-color.png");
    render2.set_3D_textures("sarco-heightmap.png", "sarco-normal.png");

}

void imgui_form(sf::RenderWindow& window) {
    ImGui::SFML::Init(window);
    ImGuiConfig config;
    config.setup();
}

void update(sf::RenderWindow& window) {
    EntityManager entity_manager;
    RenderSystem render_system {};
    PhysicsSystem physics_system {};
    CameraSystem camera_system {};

    game_entities(entity_manager);

    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;
    float fps;

    imgui_form(window);

    sf::Clock clock;
    while(window.isOpen())
    {
        start = std::chrono::high_resolution_clock::now();
        sf::Time dt = clock.restart();

        // Events
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // IMGUI
        ImGui::SFML::Update(window, clock.restart());
        ImGui::Begin("Hello, world!");
        ImGui::Text("FPS: %f", fps);
        ImGui::Text("Entities: %lu", entity_manager.get_entities_count());
        ImGui::End();

        // Systems
        camera_system.update(entity_manager, window);
        physics_system.update(entity_manager, dt.asSeconds());
        render_system.update(entity_manager, window);

        end = std::chrono::high_resolution_clock::now();
        fps = (float)1e9 / (float)std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    }

    window.close();
}


int main() {

    // To engine wrapper class
    sf::RenderWindow window;
    sf::VideoMode video_mode(1280, 800);

    sf::View view = window.getDefaultView();
    view.setSize(video_mode.width, video_mode.height);
    view.setCenter(0.f, 0.f);

    sf::ContextSettings context_settings;
    context_settings.depthBits = 24;
    context_settings.antialiasingLevel = 2;
    context_settings.sRgbCapable = false;
    context_settings.attributeFlags = sf::ContextSettings::Core;

    window.create(video_mode, "Window", sf::Style::Close, context_settings);
    window.setView(view);
    window.setVerticalSyncEnabled(true);

    update(window);

}
