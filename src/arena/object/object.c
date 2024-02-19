#include <string.h>

#include "object.h"

int
object_equals(arena_object_t *a, arena_object_t *b)
{
	return a->size == b->size && memcmp(a->data, b->data, a->size) == 0;
}

ul_t
object_hash(arena_object_t *obj)
{
	ul_t h = 0x100;
	for (size_t i = 0; i < obj->size; i++) {
		h ^= ((byte_t *)obj->data)[i];
		h *= 1111111111111111111u;
	}
	return h ^ h >> 32;
}

arena_object_t
object_insert_into_arena(arena_t *a, arena_object_t src)
{
	arena_object_t dest = { 0 };

	dest.data = arena_allocate_with_alignment(a, sizeof(byte_t),
	    _Alignof(arena_object_t), src.size, 0);

	memcpy(dest.data, src.data, src.size);

	dest.size = src.size;

	return dest;
}
