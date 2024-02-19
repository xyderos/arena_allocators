#pragma once

#include "../../error_handling/error_handling.h"
#include "../../utilities/utilities.h"

typedef struct arena {
	byte_t *original, *beg, *end;
	ul_t capacity;
	error_handler_t error_handler;
} arena_t;

/// @brief Initialize an arena with a specified capacity
/// @param capacity the capacity, defined as an unsinged long integer
/// @return The arena structure
arena_t arena_initialize(ul_t capacity);

/// @brief Reserve some memory from the arena for an object.
/// @param arena the arena to allocate the object
/// @param size the size of the object, pass in the size by using the __sizeof__
/// operator
/// @param alignment The alignment of the object, pass in the alignment by using
/// the __Alignof__ operator
/// @param count the number of objects you need to allocate memory for
/// @param flags pass context specific flags, check out the supported flags at
/// ../utilities/utilities.h
/// @return A char pointer to the reserved memory
byte_t *arena_allocate_with_alignment(arena_t *arena, ul_t size, ul_t alignment,
    ul_t count, int flags);

/// @brief Reserve some memory from the arena to be used for a sub arena
/// This is particularly useful if you want to share the memory from the arena
/// with other threads
/// @param arena the arena to allocate the object
/// @param capacity the capacity of the sub arena
/// @return A new arena to be used with the new context
arena_t arena_initialize_subarena(arena_t *arena, ul_t capacity);

/// @brief Deallocated the specified arena but be careful, if you kill a sub
/// arena it will probably cause unwanted side effects
/// @param arena the arena to deallocate
void arena_destroy(arena_t *arena);
