#include <iostream>
#include "engine/entitymanager.hpp"

struct RenderComponent {} ;

struct NameComponent {
    char name[8] {"noname"};
    std::string a;
};

struct HealthComponent {
    std::size_t health{10};
};

int main() {

    using component_types = Quark::cpp_function::Typelist<NameComponent, RenderComponent, HealthComponent>;
    using tag_types = Quark::cpp_function::Typelist<>;

    using ComponentStorageType = Quark::ComponentStorage<component_types, tag_types, 10>;

    ComponentStorageType component_storage;

    // std::cout << component_storage.get_mask<NameComponent>() << "\n";
    // std::cout << component_storage.get_mask<RenderComponent>() << "\n";
    // std::cout << component_storage.get_mask<HealthComponent>() << "\n";
    // auto& names = component_storage.get_storage<NameComponent>();
    // [[maybe_unused]] auto key = names.push_back(NameComponent{"hola"});
    
    return 1;
}
