#include <iostream>
#include "engine/entitymanager.hpp"
#include "game/types.hpp"

void print_entity(Entity const& entity) {
    std::cout << entity.has_component<NameComponent>();
    std::cout << entity.has_component<RenderComponent>();
    std::cout << entity.has_component<HealthComponent>();
    std::cout << "\n";
}

int main() {

    // using component_types = Quark::cpp_function::Typelist<NameComponent, HealthComponent, RenderComponent>;
    // using tag_types = Quark::cpp_function::Typelist<>;
    // using ComponentStorageType = Quark::ComponentStorage<component_types, tag_types, 10>;
    // ComponentStorageType component_storage;

    EntityManager entity_manager;
    Entity entity;

    auto& entity1 = entity_manager.create_entity();
    entity_manager.add_component<HealthComponent>(entity1, HealthComponent{.health = 15, .shield = 22});
    entity_manager.add_component<RenderComponent>(entity1);
    entity_manager.add_component<RenderComponent>(entity1);
    print_entity(entity1);
    
    return 1;
}
