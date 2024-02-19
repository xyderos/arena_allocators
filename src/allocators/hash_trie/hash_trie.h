#pragma once

#include <semaphore.h>

#include "../../arena/arena/arena.h"
#include "../../arena/object/object.h"

typedef struct hash_trie {
	struct hash_trie *child[4];
	arena_object_t key, value;
	int in_use;
} hash_trie_t;

void hash_trie_allocate(hash_trie_t **node, arena_t *arena, arena_object_t key,
    arena_object_t value);
hash_trie_t *hash_trie_retrieve(hash_trie_t **node, arena_object_t key);
void hash_trie_deallocate(hash_trie_t **node, arena_object_t key);
