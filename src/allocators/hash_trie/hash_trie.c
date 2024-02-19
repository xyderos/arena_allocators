#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_trie.h"

void
hash_trie_allocate(hash_trie_t **m, arena_t *arena, arena_object_t key,
    arena_object_t value)
{
	arena_t rollback;
	hash_trie_t *new_node = NULL;
	if (!arena) {
		return;
	}

	for (ul_t h = object_hash(&key);; h <<= 2) {
		hash_trie_t *n = __atomic_load_n(m, __ATOMIC_ACQUIRE);

		if (!n) {
			rollback = *arena;
			new_node = arena_allocate_with_alignment(arena,
			    sizeof(hash_trie_t), _Alignof(hash_trie_t), 1, 0);

			new_node->key = object_insert_into_arena(arena, key);
			new_node->value = object_insert_into_arena(arena,
			    value);

			new_node->in_use = 1;

			if (__atomic_compare_exchange_n(m, &n, new_node, 0,
				__ATOMIC_RELEASE, __ATOMIC_ACQUIRE)) {
				return;
			}

			*arena = rollback;
		}

		if (n->in_use && object_equals(&n->key, &key)) {
			n->value = value;
			return;
		}

		m = n->child + (h >> 62);
	}
}

hash_trie_t *
hash_trie_retrieve(hash_trie_t **m, arena_object_t key)
{
	for (ul_t h = object_hash(&key);; h <<= 2) {
		hash_trie_t *n = __atomic_load_n(m, __ATOMIC_ACQUIRE);
		if (!n) {
			return NULL;
		}
		if (n->in_use && object_equals(&n->key, &key)) {
			return n;
		}
		m = n->child + (h >> 62);
	}
}

void
hash_trie_deallocate(hash_trie_t **m, arena_object_t key)
{
	for (ul_t h = object_hash(&key);; h <<= 2) {
		hash_trie_t *n = __atomic_load_n(m, __ATOMIC_ACQUIRE);
		if (!n) {
			return;
		}
		if (object_equals(&n->key, &key)) {
			__atomic_store_n(&n->in_use, 0, __ATOMIC_RELEASE);

			memset(n->key.data, 0, n->key.size);
			n->key.size = 0;
			memset(n->value.data, 0, n->value.size);
			n->value.size = 0;
			n->in_use = 0;

			return;
		}
		m = n->child + (h >> 62);
	}
}
