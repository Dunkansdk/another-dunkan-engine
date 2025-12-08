# Template Metaprogramming

The ADE ECS relies heavily on C++ template metaprogramming to generate code at compile time. This avoids the overhead of virtual functions and runtime type checking.

## Typelist

The core abstraction is the `Typelist`, defined in `dunkan/include/ecs/utils/typelist.hpp`. It is a compile-time container for types.

```cpp
template<typename... TYPES>
struct Typelist {
    // ...
};
```

It provides helper functions like:
-   `size()`: Returns the number of types.
-   `contains<T>()`: Checks if type `T` is in the list.
-   `pos<T>()`: Returns the index of type `T` in the list.

## Meta-functions

Several meta-functions are used to manipulate types:

### `nth_type`
Retrieves the type at a specific index in a parameter pack.

### `mp_transform`
Applies a template metafunction to every type in a `Typelist`.

Example:
```cpp
// Transforms Typelist<A, B> into Typelist<Slotmap<A>, Slotmap<B>>
using storage_type = to_tuple<META_TYPES::mp_transform<to_slotmap, COMPONENT_LIST>>;
```

### `replace_t`
Replaces the container of a `Typelist`.

Example:
```cpp
// Transforms Typelist<A, B> into std::tuple<A, B>
template <typename T> using to_tuple = META_TYPES::replace_t<std::tuple, T>;
```

## Component Storage Generation

The `ComponentStorage` class uses these tools to generate the storage layout:

```cpp
using storage_type = to_tuple<META_TYPES::mp_transform<to_slotmap, COMPONENT_LIST>>;
```

1.  `COMPONENT_LIST` is a `Typelist<Pos, Vel, ...>`.
2.  `mp_transform` applies `to_slotmap` to each type, resulting in `Typelist<Slotmap<Pos>, Slotmap<Vel>, ...>`.
3.  `to_tuple` converts this `Typelist` into `std::tuple<Slotmap<Pos>, Slotmap<Vel>, ...>`.

This allows `ComponentStorage` to hold exactly the right `SlotMap`s for the defined components, all determined at compile time.
