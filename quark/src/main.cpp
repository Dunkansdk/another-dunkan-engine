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

    using component_types = Quark::cpp_function::Typelist<NameComponent, RenderComponent, HealthComponent>;
    using tag_types = Quark::cpp_function::Typelist<>;
    using ComponentStorageType = Quark::ComponentStorage<component_types, tag_types, 10>;
    ComponentStorageType component_storage;

    EntityManager entity_manager;
    Entity entity;

    auto& entity1 = entity_manager.create_entity();
    auto& component1 = entity_manager.add_component<NameComponent>(entity1);
    auto& component2 = entity_manager.add_component<HealthComponent>(entity1);
    auto& component3 = entity_manager.add_component<RenderComponent>(entity1);
    auto& component4 = entity_manager.add_component<HealthComponent>(entity1);
    if(&component2 == &component4) std::cout << "Equal component\n";
    print_entity(entity1);

    std::cout << ComponentStorageType::component_info::mask<NameComponent>() << "\n";
    std::cout << ComponentStorageType::component_info::mask<RenderComponent>() << "\n";
    std::cout << ComponentStorageType::component_info::mask<HealthComponent>() << "\n";
    auto& names = component_storage.get_storage<NameComponent>();
    [[maybe_unused]] auto key = names.push_back(NameComponent{"hola"});
    
    return 1;
}
