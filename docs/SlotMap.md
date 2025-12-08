# SlotMap

The `SlotMap` is the underlying data structure used by the ECS to store components. It provides O(1) insertion, deletion, and access, while maintaining stable indices (keys) even when elements are moved in memory.

## Overview

A `SlotMap` solves the "ABA problem" and pointer invalidation issues common in dynamic arrays. When an object is removed, its slot can be reused, but the `SlotMap` ensures that old keys cannot access the new object in that slot.

## Implementation Details

The `SlotMap` is implemented in `dunkan/include/ecs/utils/slotmap.hpp`.

### Data Structures

It uses three main arrays:
1.  **`m_data`**: A dense array storing the actual objects (`value_type`). This ensures cache locality.
2.  **`m_index`**: A sparse array mapping keys to indices in `m_data`.
3.  **`m_erase`**: An array mapping indices in `m_data` back to keys (used for efficient deletion).

### Keys and Generations

A `key_type` consists of:
-   `id`: The index in the `m_index` array.
-   `generation`: A version counter.

Each slot in `m_index` also stores a `generation`. When a key is used to access data:
1.  The `id` is used to look up the slot in `m_index`.
2.  The `generation` in the key is compared with the `generation` in the slot.
3.  If they match, the access is valid. If not, the key is stale (the object was deleted).

### Allocation (Push Back)

When `push_back` is called:
1.  A slot is allocated from the free list.
2.  The object is moved into `m_data` at the end (`m_size`).
3.  The `m_index` slot is updated to point to this new location in `m_data`.
4.  A new key is returned with the current generation.

### Deletion (Erase)

When `erase` is called:
1.  The key is validated.
2.  The object in `m_data` is removed. To keep `m_data` packed, the last object in `m_data` is moved to the empty spot (swap-and-pop).
3.  The `m_index` and `m_erase` arrays are updated to reflect the move.
4.  The generation of the freed slot in `m_index` is incremented, invalidating all existing keys to that slot.
5.  The slot is added to the free list.

## Benefits

-   **Cache Friendliness**: All active components are stored contiguously in `m_data`.
-   **Safety**: Stale keys cannot access reused memory slots.
-   **Performance**: All operations are O(1).
