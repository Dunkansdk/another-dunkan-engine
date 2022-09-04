#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>

#include "engine/entitymanager.hpp"
#include "game/imguiconfig.hpp"
#include "game/types.hpp"

void print_entity(Entity const& entity) {
    std::cout << entity.has_component<PhysicsComponent>();
    std::cout << entity.has_component<RenderComponent>();
    std::cout << entity.has_component<NameComponent>();
    std::cout << "\n";
}

void game_entities() {
    EntityManager entity_manager;
    Entity entity;

    auto& entity1 = entity_manager.create_entity();
    PhysicsComponent& physics_component_1 = entity_manager.add_component<PhysicsComponent>(entity1, PhysicsComponent{.x = 69.f, .y = 69.f, .z = 1.f});
    entity_manager.add_component<RenderComponent>(entity1);
    NameComponent& name_component_1 = entity_manager.add_component<NameComponent>(entity1, NameComponent{"qweqwe"});
    print_entity(entity1);

    auto& entity2 = entity_manager.create_entity();
    NameComponent& name_component_2 = entity_manager.add_component<NameComponent>(entity2, NameComponent{"asdasd"});
    entity_manager.add_component<RenderComponent>(entity2);
    entity_manager.add_component<RenderComponent>(entity2);
    print_entity(entity2);

    std::cout << "Entity 1: \n";
    std::cout << "PhysicsComponent&.x -> " << physics_component_1.x << "\n";
    std::cout << "NameComponent&.x -> " << name_component_1.name << "\n";

    std::cout << "Entity 2: \n";
    std::cout << "NameComponent&.x -> " << name_component_2.name << "\n";
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "ImGui + SFML = <3");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    ImGuiConfig config;
    config.setup();
    
    // Debugger Game Entities
    game_entities();

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::ShowDemoWindow();

        window.clear();
        // window.draw();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}