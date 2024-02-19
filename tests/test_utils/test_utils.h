#pragma once

#include "../../src/allocators/hash_trie/hash_trie.h"
#include "../../src/allocators/probing_map/probing_map.h"

typedef struct coordinates {
	ul_t x, y;
} coordinates_t;

typedef struct {
	hash_trie_t **trie;
	probing_map_t *map;
	arena_t arena;
	ul_t start, stop;
} thread_context_t;
