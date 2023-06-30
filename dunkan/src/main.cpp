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

#include "utils/Configuration.hpp"

#include "game/imguiconfig.hpp"


unsigned int m_frame;
unsigned int m_fps;
sf::Clock m_clock;

bool game_entities(EntityManager& entity_manager) {

    TextureManager texture_manager {};

    texture_manager.load(std::string("Abbey-Albedo"), std::string("data/abbey_albedo.png"));
    texture_manager.load(std::string("Abbey-Depth"), std::string("data/abbey_height.png"));
    texture_manager.load(std::string("Abbey-Normal"), std::string("data/abbey_normal.png"));

    texture_manager.load(std::string("Tree-Albedo"), std::string("data/tree_albedo.png"));
    texture_manager.load(std::string("Tree-Depth"), std::string("data/tree_height.png"));
    texture_manager.load(std::string("Tree-Normal"), std::string("data/tree_normal.png"));
    texture_manager.load(std::string("Tree-Material"), std::string("data/tree_material.png"));

    texture_manager.load(std::string("Torus-Albedo"), std::string("data/torus_albedo.png"));
    texture_manager.load(std::string("Torus-Depth"), std::string("data/torus_height.png"));
    texture_manager.load(std::string("Torus-Normal"), std::string("data/torus_normal.png"));
    texture_manager.load(std::string("Torus-Material"), std::string("data/torus_material.png"));

    texture_manager.load(std::string("Wetsand-Albedo"), std::string("data/wetsand_albedo.png"));
    texture_manager.load(std::string("Wetsand-Depth"), std::string("data/wetsand_height.png"));
    texture_manager.load(std::string("Wetsand-Normal"), std::string("data/wetsand_normal.png"));
    texture_manager.load(std::string("Wetsand-Material"), std::string("data/wetsand_normal.png"));

    texture_manager.load(std::string("Sarco-Albedo"), std::string("data/sarco_albedo.png"));
    texture_manager.load(std::string("Sarco-Depth"), std::string("data/sarco_height.png"));
    texture_manager.load(std::string("Sarco-Normal"), std::string("data/sarco_normal.png"));

    Entity& entity1 = entity_manager.create_entity();
    entity_manager.add_component<PhysicsComponent>(entity1, PhysicsComponent{
            .x = 350.f,
            .y = 10.f,
            .z = .5f
        });
    entity_manager.add_component<RenderComponent>(entity1, RenderComponent{
            texture_manager.get("Abbey-Albedo"), 
            sf::IntRect(0, 0, texture_manager.get("Abbey-Albedo").getSize().x, texture_manager.get("Abbey-Albedo").getSize().y),
            460.f,
            1.f,
            texture_manager.get("Abbey-Normal"), 
            texture_manager.get("Abbey-Depth")
        }).load();

    Entity& entity2 = entity_manager.create_entity();
    entity_manager.add_component<PhysicsComponent>(entity2, PhysicsComponent{
            .x = 140.f,
            .y = 180.f,
            .z = -.65f
        });
    entity_manager.add_component<RenderComponent>(entity2, RenderComponent{
            texture_manager.get("Tree-Albedo"), 
            sf::IntRect(0, 0, texture_manager.get("Tree-Albedo").getSize().x, texture_manager.get("Tree-Albedo").getSize().y),
            300.f,
            1.f,
            texture_manager.get("Tree-Normal"), 
            texture_manager.get("Tree-Depth"), 
            texture_manager.get("Tree-Material")
        }).load();

    // Sunlight
    Entity& entity3 = entity_manager.create_entity();
    entity_manager.add_component<PhysicsComponent>(entity3, PhysicsComponent{
            .x = 900.f,
            .y = 900.f,
            .z = .5f
        });
    entity_manager.add_component<LightComponent>(entity3, LightComponent{
            .light_type = LightType::DIRECTIONAL,
            .diffuse_color = sf::Color(255,255,224),
            .direction = sf::Vector3f(-1,-.2,-1),
            .intensity = 5.0f
        });

    // Spotlight
    Entity& entity4 = entity_manager.create_entity();
    entity_manager.add_component<PhysicsComponent>(entity4, PhysicsComponent{
            .x = 500.f,
            .y = 200.f,
            .z = 3.f
        });
    entity_manager.add_component<LightComponent>(entity4, LightComponent{
            .light_type = LightType::SPOT,
            .diffuse_color = sf::Color(255,190,64),
            .direction = sf::Vector3f(0, 1.0, 0),
            .radius = 3.0f,
            .intensity = 10.0f
        });

    Entity& entity5 = entity_manager.create_entity();
    entity_manager.add_component<PhysicsComponent>(entity5, PhysicsComponent{
            .x = -512.f,
            .y = -224.f,
            .z = .5f
        });
    entity_manager.add_component<RenderComponent>(entity5, RenderComponent{
            texture_manager.get("Wetsand-Albedo"), 
            sf::IntRect(0, 0, texture_manager.get("Wetsand-Albedo").getSize().x * 7, texture_manager.get("Wetsand-Albedo").getSize().y * 7),
            10.f,
            1.f,
            texture_manager.get("Wetsand-Normal"), 
            texture_manager.get("Wetsand-Depth"),
            texture_manager.get("Wetsand-Material"),
            false
        }).load();

    Entity& entity6 = entity_manager.create_entity();
    entity_manager.add_component<PhysicsComponent>(entity6, PhysicsComponent{
            .x = 300.f,
            .y = 300.f,
            .z = -.65f
        });
    entity_manager.add_component<RenderComponent>(entity6, RenderComponent{
            texture_manager.get("Torus-Albedo"), 
            sf::IntRect(0, 0, texture_manager.get("Torus-Albedo").getSize().x, texture_manager.get("Torus-Albedo").getSize().y),
            40.f,
            1.f,
            texture_manager.get("Torus-Normal"), 
            texture_manager.get("Torus-Depth"), 
            texture_manager.get("Torus-Material")
        }).load();

    Entity& entity7 = entity_manager.create_entity();
    entity_manager.add_component<PhysicsComponent>(entity7, PhysicsComponent{
            .x = 540.f,
            .y = 580.f,
            .z = -.65f
        });
    entity_manager.add_component<RenderComponent>(entity7, RenderComponent{
            texture_manager.get("Tree-Albedo"), 
            sf::IntRect(0, 0, texture_manager.get("Tree-Albedo").getSize().x, texture_manager.get("Tree-Albedo").getSize().y),
            300.f,
            1.f,
            texture_manager.get("Tree-Normal"), 
            texture_manager.get("Tree-Depth"), 
            texture_manager.get("Tree-Material")
        }).load();

    Entity& entity8 = entity_manager.create_entity();
    entity_manager.add_component<PhysicsComponent>(entity8, PhysicsComponent{
            .x = 450.f,
            .y = 200.f,
            .z = .5f
        });
    entity_manager.add_component<RenderComponent>(entity8, RenderComponent{
            texture_manager.get("Sarco-Albedo"), 
            sf::IntRect(0, 0, texture_manager.get("Sarco-Albedo").getSize().x, texture_manager.get("Sarco-Albedo").getSize().y),
            85.f,
            1.f,
            texture_manager.get("Sarco-Normal"), 
            texture_manager.get("Sarco-Depth")
        }).load();

    Configuration::load(entity_manager);

    return true;
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

    if(game_entities(entity_manager)) {

        if(!render_load) {
            render_system.init_renderer(window);
            render_load = true;
        }

        imgui_form(window);
    
        sf::Clock clock;
        sf::Event event;
        while(window.isOpen())
        {
            sf::Time dt = clock.restart();

            while (window.pollEvent(event)) 
            {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }

                if (event.type == sf::Event::Resized)
                {
                    sf::FloatRect visible_area(0.f, 0.f, event.size.width, event.size.height);
                    window.setView(sf::View(visible_area));
                }

                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
                        render_system.debug_screen = 0;
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
                        render_system.debug_screen = 1;
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
                        render_system.debug_screen = 2;
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::V))
                        render_system.debug_screen = 3;

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
            if (ImGui::Button("SSAO")) {
                render_system.SSAO_option(!Configuration::get()->enable_SSAO);
            }
            if (ImGui::Button("Gamma Correction")) {
                render_system.enable_gamma_correction(!Configuration::get()->enable_SRGB);
            }
            ImGui::End();
            debug_system.update(entity_manager);
#endif

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

        ImGui::SFML::Shutdown();
        render_system.destroy();
        window.close();

    }
}


int main() {
    sf::RenderWindow window;
    sf::VideoMode video_mode(1920, 1080, 32);

    sf::View view = window.getView();
    view.setSize(video_mode.width, video_mode.height);
    view.setCenter(video_mode.width / 2, video_mode.height / 2);

    sf::ContextSettings context_settings;
    context_settings.depthBits = 24;
    context_settings.stencilBits = 8;
    context_settings.antialiasingLevel = 4;
    context_settings.attributeFlags = sf::ContextSettings::Core;

    window.create(video_mode, "Window", sf::Style::Close | sf::Style::Resize, context_settings);
    window.setView(view);

    update(window);
}
