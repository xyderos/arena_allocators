#pragma once

#include "../../utilities/utilities.h"
#include "../arena/arena.h"

typedef struct {
	byte_t *data;
	ul_t size;
} arena_object_t;

int object_equals(arena_object_t *a, arena_object_t *b);

ul_t object_hash(arena_object_t *obj);

arena_object_t object_insert_into_arena(arena_t *a, arena_object_t src);
