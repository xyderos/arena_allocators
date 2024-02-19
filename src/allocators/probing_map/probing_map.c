#include <string.h>

#include "probing_map.h"

static ul_t
hash_to_index(arena_object_t key, ul_t capacity)
{
	ul_t hash = 0;
	byte_t *key_bytes = (byte_t *)key.data;

	for (ul_t i = 0; i < key.size; ++i) {
		hash = hash * 31 + key_bytes[i];
	}

	return hash % capacity;
}

static void
upsert(probing_map_t *map, arena_object_t key, arena_object_t value)
{
	size_t index = hash_to_index(key, map->capacity);

	while (1) {
		if (!__atomic_load_n(&map->slots[index].in_use,
			__ATOMIC_ACQUIRE) ||
		    object_equals(&map->slots[index].key, &key)) {
			break;
		}
		index = (index + 1) % map->capacity;
	}

	while (!__atomic_compare_exchange_n(&map->slots[index].in_use,
	    &(int) { 0 }, 1, 0, __ATOMIC_RELEASE, __ATOMIC_ACQUIRE)) {
	}

	map->slots[index].key = object_insert_into_arena(&map->arena, key);
	map->slots[index].value = object_insert_into_arena(&map->arena, value);

	__atomic_store_n(&map->slots[index].in_use, 1, __ATOMIC_RELEASE);
}

arena_object_t
probing_map_allocate(probing_map_t *map, arena_object_t value)
{
	ul_t *key_data;
	arena_object_t key = { 0 };

	sem_wait(&map->lock);

	key_data = (size_t *)arena_allocate_with_alignment(&map->arena,
	    sizeof(size_t), _Alignof(size_t), 1, 0);
	*key_data = __atomic_fetch_add(&map->key_counter, 1, __ATOMIC_RELAXED);

	key.data = key_data;
	key.size = sizeof(size_t);

	upsert(map, key, value);
	sem_post(&map->lock);
	return key;
}

map_slot_t *
probing_map_retrieve(probing_map_t *map, arena_object_t key)
{
	ul_t index;

	sem_wait(&map->lock);
	index = hash_to_index(key, map->capacity);

	while (__atomic_load_n(&map->slots[index].in_use, __ATOMIC_ACQUIRE)) {
		if (object_equals(&map->slots[index].key, &key)) {
			sem_post(&map->lock);
			return &map->slots[index];
		}
		index = (index + 1) % map->capacity;
	}
	sem_post(&map->lock);
	return NULL;
}

void
probing_map_deallocate(probing_map_t *map, arena_object_t key)
{
	ul_t index;

	sem_wait(&map->lock);
	index = hash_to_index(key, map->capacity);

	while (__atomic_load_n(&map->slots[index].in_use, __ATOMIC_ACQUIRE)) {
		if (object_equals(&map->slots[index].key, &key)) {
			__atomic_store_n(&map->slots[index].in_use, 0,
			    __ATOMIC_RELEASE);

			memset(map->slots[index].key.data, 0,
			    map->slots[index].key.size);
			memset(map->slots[index].value.data, 0,
			    map->slots[index].value.size);
			map->slots[index].key.size = 0;
			map->slots[index].value.size = 0;
			sem_post(&map->lock);
			return;
		}
		index = (index + 1) % map->capacity;
	}
	sem_post(&map->lock);
}
