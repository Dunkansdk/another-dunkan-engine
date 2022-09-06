#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>

#include "engine/entitymanager.hpp"
#include "engine/utils/typelist.hpp"
#include "game/components/namecomponent.hpp"
#include "game/components/physicscomponent.hpp"
#include "game/components/rendercomponent.hpp"
#include "game/imguiconfig.hpp"
#include "game/types.hpp"

void print_entity(Entity const& entity) {
    std::cout << entity.has_component<PhysicsComponent>();
    std::cout << entity.has_component<RenderComponent>();
    std::cout << entity.has_component<NameComponent>();
    std::cout << "\n";
    std::cout << "Expand Variadic Template: has_components\n<PhysicsComponent, NameComponent, RenderComponent> -> " << entity.has_components<PhysicsComponent, NameComponent, RenderComponent>() << "\n";
}

void game_entities(EntityManager& entity_manager, Entity& entity) {

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

    EntityManager entity_manager;
    Entity entity;
    // Debugger Game Entities
    game_entities(entity_manager, entity);

    using RenderSystem_t = ADE::META_TYPES::Typelist<RenderComponent, NameComponent>;


    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {

            entity_manager.forEntitiesMatching<RenderSystem_t> 
                ([](auto, auto& render_component, auto& name_component)
                {
                    std::cout << name_component.name << "\n";
                });
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();
        // window.draw();
        window.display();
    }


    return 0;
}
