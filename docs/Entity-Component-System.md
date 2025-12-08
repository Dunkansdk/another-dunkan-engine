# Entity Component System (ECS)

The Another Dunkan Engine (ADE) features a custom, high-performance Entity Component System (ECS). It is designed to be cache-friendly and type-safe, leveraging C++ templates to define the system architecture at compile time.

## Core Concepts

### EntityManager

The `EntityManager` is the central hub of the ECS. It manages the lifecycle of entities and coordinates the storage and retrieval of components.

It is defined as a template class:
```cpp
template <typename COMPONENT_LIST, typename SINGLETON_LIST, typename TAG_LIST = META_TYPES::Typelist<>, std::size_t CAPACITY = 10>
struct EntityManager;
```

- **COMPONENT_LIST**: A `Typelist` of all component types supported by this manager.
- **SINGLETON_LIST**: A `Typelist` of singleton components (components that exist only once per manager).
- **TAG_LIST**: A `Typelist` of tags (empty structs used for flagging entities).
- **CAPACITY**: The default capacity for the internal `SlotMap`s.

### Entity

An `Entity` in ADE is not a heavy object but a lightweight handle. It contains:
- A unique ID.
- A mask indicating which components it possesses.
- A set of keys (indices) into the component storage.

Entities are created via `EntityManager::create_entity()`.

### Component Storage

Components are stored in `ComponentStorage`, which internally uses a `std::tuple` of `SlotMap`s. There is one `SlotMap` for each component type in `COMPONENT_LIST`. This ensures that components of the same type are stored contiguously in memory, improving cache locality during iteration.

## Usage

### Defining Components

Components are simple structs:

```cpp
struct Position { float x, y; };
struct Velocity { float dx, dy; };
```

### Defining the Manager

You define your `EntityManager` type by specifying the components:

```cpp
using MyComponents = ADE::META_TYPES::Typelist<Position, Velocity>;
using MySingletons = ADE::META_TYPES::Typelist<>;
using MyManager = ADE::EntityManager<MyComponents, MySingletons>;
```

### Working with Entities

```cpp
MyManager manager;
auto& entity = manager.create_entity();

// Add components
manager.add_component<Position>(entity, 10.0f, 20.0f);
manager.add_component<Velocity>(entity, 1.0f, 0.0f);

// Get components
if (entity.has_component<Position>()) {
    auto& pos = manager.get_component<Position>(entity);
    pos.x += 5.0f;
}
```

### Iteration

The `EntityManager` provides `foreach` to iterate over entities with specific components:

```cpp
// Update all entities with Position and Velocity
manager.foreach<ADE::META_TYPES::Typelist<Position, Velocity>, ADE::META_TYPES::Typelist<>>([](auto& entity, Position& pos, Velocity& vel) {
    pos.x += vel.dx;
    pos.y += vel.dy;
});
```
