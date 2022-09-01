#include <iostream>
#include "engine/entitymanager.hpp"

struct RenderComponent {} ;

struct NameComponent {
    char name[8] {"noname"};
};

struct HealthComponent {
    std::size_t health{10};
};

struct Entity {};

int main() {

    using component_types = Quark::cpp_function::Typelist<NameComponent, RenderComponent, HealthComponent>;
    using tag_types = Quark::cpp_function::Typelist<>;
    using ComponentStorageType = Quark::ComponentStorage<component_types, tag_types, 10>;
    ComponentStorageType component_storage;
    Quark::EntityManager<Entity, HealthComponent, NameComponent, RenderComponent> entity_manager;
    Quark::EntityManager<Entity, HealthComponent, NameComponent, RenderComponent>::Entity entity;

    std::cout << ComponentStorageType::component_info::mask<NameComponent>() << "\n";
    std::cout << ComponentStorageType::component_info::mask<RenderComponent>() << "\n";
    std::cout << ComponentStorageType::component_info::mask<HealthComponent>() << "\n";
    auto& names = component_storage.get_storage<NameComponent>();
    [[maybe_unused]] auto key = names.push_back(NameComponent{"hola"});
    
    return 1;
}
