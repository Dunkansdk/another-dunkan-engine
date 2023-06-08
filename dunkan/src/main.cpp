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
    entity_manager.add_component<PhysicsComponent>(entity1, PhysicsComponent{
        .x = 350.f,
        .y = 10.f,
        .z = 0.5f
    });
    RenderComponent& render = entity_manager.add_component<RenderComponent>(entity1, RenderComponent{});
    render.set_texture("data/abbey_albedo.png");
    render.set_3D_textures("data/abbey_height.png", "data/abbey_normal.png");

    Entity& entity2 = entity_manager.create_entity();
    entity_manager.add_component<PhysicsComponent>(entity2, PhysicsComponent{
        .x = 140.f,
        .y = 180.f,
        .z = 0.5
    });
    RenderComponent& render2 = entity_manager.add_component<RenderComponent>(entity2, RenderComponent{});
    render2.set_texture("data/tree_albedo.png");
    render2.set_3D_textures("data/tree_height.png", "data/tree_normal.png");

    // Sunlight
    Entity& entity3 = entity_manager.create_entity();
    entity_manager.add_component<PhysicsComponent>(entity3, PhysicsComponent{
        .x = 900.f,
        .y = 900.f,
        .z = .5f
    });
    entity_manager.add_component<LightComponent>(entity3, LightComponent{
        .diffuse_color = sf::Color(255,255,224),
        .specular_color = sf::Color::White,
        .direction = sf::Vector3f(-1,.5,-1),
        .contant_attenuation = .9f,
        .quadratic_attenuation = 0.00001,
        .linear_attenuation = 0.00001,
        .global_light = true
    });

    // Spotlight
    Entity& entity4 = entity_manager.create_entity();
    entity_manager.add_component<PhysicsComponent>(entity4, PhysicsComponent{
        .x = 500.f,
        .y = 200.f,
        .z = .5f
    });
    entity_manager.add_component<LightComponent>(entity4, LightComponent{
        .diffuse_color = sf::Color::Red,
        .specular_color = sf::Color::White,
        .direction = sf::Vector3f(0, 0, 1),
        .quadratic_attenuation = 0.00001,
        .linear_attenuation = 0.000001
    });
}

void imgui_form(sf::RenderWindow& window) {
    ImGui::SFML::Init(window);
    ImGuiConfig config;
    config.setup();
}

bool render_load = false;

void update(sf::RenderWindow& window) {
    EntityManager entity_manager;
    RenderSystem render_system {};
    PhysicsSystem physics_system {};
    DebugSystem debug_system {}; 
    CameraSystem camera_system {};
    MoveEntitySystem move_entity_system {};

    if(!render_load) {
        render_system.init_renderer(window);
        render_load = true;
    }

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

#ifdef DEBUG_IMGUI
            ImGui::SFML::ProcessEvent(window, event);
            // System poll events
            move_entity_system.update(entity_manager, window, event);
#endif
        }

#ifdef DEBUG_IMGUI
        ImGui::SFML::Update(window, clock.restart());
        ImGui::Begin("Hello, world!");
        ImGui::Text("FPS: %f", (float)m_fps);
        ImGui::Text("Entities: %lu", entity_manager.get_entities_count());
        ImGui::Checkbox("Heightmaps", &render_system.debug_heightmap);
        ImGui::Checkbox("SSAO", &render_system.m_enableSSAO);
        ImGui::End();
        debug_system.update(entity_manager);
#endif

        // Systems
        camera_system.update(window, dt.asSeconds());
        physics_system.update(entity_manager, dt.asSeconds());
        render_system.update(entity_manager, window);

        window.display();


        if(m_clock.getElapsedTime().asSeconds() >= 1.f)
		{
			m_fps = m_frame;
			m_frame = 0;
			m_clock.restart();
		}
 
		++m_frame;
    }

    render_system.destroy();
    window.close();
}


int main() {
    sf::RenderWindow window;
    sf::VideoMode video_mode(1920, 1080, 32);

    sf::View view = window.getDefaultView();
    view.setSize(video_mode.width, video_mode.height);
    view.setCenter(video_mode.width / 2, video_mode.height / 2);

    sf::ContextSettings context_settings;
    context_settings.depthBits = 24;
    context_settings.stencilBits = 8;
    context_settings.antialiasingLevel = 2;
    //context_settings.attributeFlags = sf::ContextSettings::Core;

    window.create(video_mode, "Window", sf::Style::Close | sf::Style::Titlebar, context_settings);
    window.setView(view);
    // window.setVerticalSyncEnabled(true);
    // window.setFramerateLimit(165);

    update(window);

}
