#pragma once
#include <semaphore.h>

#include "../../arena/object/object.h"

typedef struct {
	arena_object_t key;
	arena_object_t value;
	int in_use;
} map_slot_t;

typedef struct {
	map_slot_t *slots;
	ul_t capacity, key_counter;
	arena_t arena;
	sem_t lock;
} probing_map_t;

arena_object_t probing_map_allocate(probing_map_t *map, arena_object_t value);
map_slot_t *probing_map_retrieve(probing_map_t *map, arena_object_t key);
void probing_map_deallocate(probing_map_t *map, arena_object_t key);
