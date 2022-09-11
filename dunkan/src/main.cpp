#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>

#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/WindowStyle.hpp"
#include "engine/entitymanager.hpp"
#include "engine/utils/typelist.hpp"
#include "game/components/namecomponent.hpp"
#include "game/components/physicscomponent.hpp"
#include "game/components/rendercomponent.hpp"
#include "game/systems/physicssystem.hpp"
#include "game/types.hpp"
#include "game/systems/rendersystem.hpp"

void print_entity(Entity const& entity) {
    std::cout << entity.has_component<PhysicsComponent>();
    std::cout << entity.has_component<RenderComponent>();
    std::cout << entity.has_component<NameComponent>();
    std::cout << "\n";
    //std::cout << "Expand Variadic Template: has_components\n<PhysicsComponent, NameComponent, RenderComponent> -> " << entity.has_components<PhysicsComponent, NameComponent, RenderComponent>() << "\n";
}

void repeat(EntityManager& entity_manager) {

    sf::CircleShape circle(50.0f);
    circle.setFillColor(sf::Color(255, 255, 255));
    auto& entity = entity_manager.create_entity();
    entity_manager.add_component<NameComponent>(entity, NameComponent{"asdasd"});
    entity_manager.add_component<RenderComponent>(entity, RenderComponent{.circle = circle});
    entity_manager.add_component<PhysicsComponent>(entity, PhysicsComponent{.x = -10.f, .y = -10.f, .z = 1.f, .velocity_x = -0.5f});
    //print_entity(entity);

    std::cout << entity_manager.get_entities_count() << std::endl;

}

void game_entities(EntityManager& entity_manager) {

    sf::CircleShape circle(50.0f);
    circle.setFillColor(sf::Color(255, 255, 255));

    auto& entity1 = entity_manager.create_entity();
    entity_manager.add_component<PhysicsComponent>(entity1, PhysicsComponent{.x = -69.f, .y = -69.f, .z = 1.f});
    entity_manager.add_component<RenderComponent>(entity1, RenderComponent{.circle = circle});
    entity_manager.add_component<NameComponent>(entity1, NameComponent{"qweqwe"});
    print_entity(entity1);

    auto& entity2 = entity_manager.create_entity();
    entity_manager.add_component<NameComponent>(entity2, NameComponent{"asdasd"});
    entity_manager.add_component<RenderComponent>(entity2, RenderComponent{.circle = circle});
    entity_manager.add_component<PhysicsComponent>(entity2, PhysicsComponent{.x = -10.f, .y = -10.f, .z = 1.f, .velocity_x = -0.5f});
    print_entity(entity2);

    // Entity without physics component
    auto& entity3 = entity_manager.create_entity();
    entity_manager.add_component<RenderComponent>(entity3, RenderComponent { .circle = circle });
    entity_manager.add_component<PhysicsComponent>(entity3);
    entity_manager.add_component<NameComponent>(entity3, NameComponent{"qweqwe"});
    print_entity(entity3);

    bool success = entity_manager.erase_component<NameComponent>(entity2);
    entity_manager.get_component<RenderComponent>(entity2);
    entity_manager.get_component<PhysicsComponent>(entity2);
    print_entity(entity2);
    std::cout << "Removed component: " << success << "\n";


    success = entity_manager.erase_entity(entity3);
    print_entity(entity3);
    std::cout << "Removed entity: " << success << "\n";
}

int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(800, 600), "Window", sf::Style::Default, settings);
    window.setFramerateLimit(60);

    EntityManager entity_manager;
    game_entities(entity_manager);

    RenderSystem render_system {};
    PhysicsSystem physics_system {};

    sf::Clock deltaClock;
    while (window.isOpen()) {
        physics_system.update(entity_manager);
        render_system.update(entity_manager, window);
        repeat(entity_manager);
    }

    return 0;
}
