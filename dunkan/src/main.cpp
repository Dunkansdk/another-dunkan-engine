#include <iostream>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <type_traits>
#include "SFML/Window/ContextSettings.hpp"
#include "game/systems/camerasystem.hpp"
#include "game/systems/debugsystem.hpp"
#include "game/types.hpp"
#include "game/systems/physicssystem.hpp"
#include "game/systems/rendersystem.hpp"
#include "game/systems/moveentitysystem.hpp"
#include "game/imguiconfig.hpp"

unsigned int m_frame;
unsigned int m_fps;
sf::Clock m_clock;

void game_entities(EntityManager& entity_manager) {

    Entity& entity1 = entity_manager.create_entity();
    entity_manager.add_component<PhysicsComponent>(entity1, PhysicsComponent{.x = 350.f, .y = 10.f, .z = 0.f});
    RenderComponent& render = entity_manager.add_component<RenderComponent>(entity1, RenderComponent{});
    render.set_texture("data/abbey_albedo.png");
    render.set_3D_textures("data/abbey_height.png", "data/abbey_normal.png");

    Entity& entity2 = entity_manager.create_entity();
    entity_manager.add_component<PhysicsComponent>(entity2, PhysicsComponent{ .x = 140.f, .y = 180.f, .z = 0.f});
    RenderComponent& render2 = entity_manager.add_component<RenderComponent>(entity2, RenderComponent{});
    render2.set_texture("data/tree_albedo.png");
    render2.set_3D_textures("data/tree_height.png", "data/tree_normal.png");

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
    DebugSystem debug_system {}; 
    CameraSystem camera_system {};
    MoveEntitySystem move_entity_system {};

    game_entities(entity_manager);

    imgui_form(window);
    
    sf::Clock clock;
    while(window.isOpen())
    {
        sf::Time dt = clock.restart();

        sf::Event event;
        while (window.pollEvent(event)) 
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // System poll events
            move_entity_system.update(entity_manager, window, event);
        }

#ifdef DEBUG_IMGUI
        ImGui::SFML::ProcessEvent(window, event);
#endif

#ifdef DEBUG_IMGUI
        ImGui::SFML::Update(window, clock.restart());
        ImGui::Begin("Hello, world!");
        ImGui::Text("FPS: %f", (float)m_fps);
        ImGui::Text("Entities: %lu", entity_manager.get_entities_count());
        ImGui::Checkbox("Heightmaps", &render_system.debug_heightmap);
        ImGui::End();
#endif

        // Systems
        camera_system.update(window, dt.asSeconds());
        debug_system.update(entity_manager);
        physics_system.update(entity_manager, dt.asSeconds());
        render_system.update(entity_manager, window);

        if(m_clock.getElapsedTime().asSeconds() >= 1.f)
		{
			m_fps = m_frame;
			m_frame = 0;
			m_clock.restart();
		}
 
		++m_frame;
    }

    window.close();
}


int main() {

    // To engine wrapper class
    sf::RenderWindow window;
    sf::VideoMode video_mode(1920, 1080, 32);

    sf::View view = window.getDefaultView();
    view.setSize(video_mode.width, video_mode.height);
    view.setCenter(video_mode.width / 2, video_mode.height / 2);

    sf::ContextSettings context_settings;
    context_settings.depthBits = 24;
    context_settings.antialiasingLevel = 1;
    context_settings.attributeFlags = sf::ContextSettings::Core;

    window.create(video_mode, "Window", sf::Style::Close | sf::Style::Titlebar, context_settings);
    window.setView(view);
    // window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(165);

    update(window);

}
