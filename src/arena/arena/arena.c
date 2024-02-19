#include <sys/mman.h>

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arena.h"

static arena_t
arena_initialize_(ul_t capacity, error_handler_t *error_handler)
{
	arena_t arena = { 0 };

	arena.original = mmap(NULL, capacity, PROT_READ | PROT_WRITE,
	    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (arena.original == MAP_FAILED) {
		jump_back(error_handler, INTERNAL_ALLOCATOR_CREATION_ERROR);
	}

	arena.beg = arena.original;
	arena.end = arena.original + capacity;
	arena.capacity = capacity;
	return arena;
}

arena_t
arena_initialize(ul_t capacity)
{
	error_handler_t env;
	env.is_set = RESET_ERROR_HANDLER;

	if (set_jump_point(&env) == NO_ERROR) {
		return arena_initialize_(capacity, &env);
	} else {
		if (env.error_code == INTERNAL_ALLOCATOR_CREATION_ERROR) {
			(void)fprintf(stderr,
			    "Failed to initialize arena due to memory allocation error.\n");
		} else {
			(void)fprintf(stderr,
			    "Failed to initialize arena due to another error.\n");
		}
		abort();
	}
}

static byte_t *__attribute((malloc, alloc_size(2, 4), alloc_align(3)))
arena_allocate_with_alignment_(arena_t *arena, ul_t size, ul_t alignment,
    ul_t count, int flags)
{
	byte_t *r;
	ul_t available = (ul_t)(arena->end - arena->beg);
	ul_t padding = -(ul_t)arena->beg & (alignment - 1);

	if (count > (available - padding) / size) {
		if (flags & SOFT_FAIL_FLAGS) {
			return NULL;
		}
		jump_back(&arena->error_handler, MEMORY_ALLOCATION_ERROR);
	}

	arena->beg += padding;
	r = arena->beg;
	arena->beg += size * count;
	return flags & ZERO_ALLOCATED_FLAGS ? memset(r, 0, size * count) : r;
}

byte_t *__attribute((malloc, alloc_size(2, 4), alloc_align(3)))
arena_allocate_with_alignment(arena_t *arena, ul_t size, ul_t align, ul_t count,
    int flags)
{
	arena->error_handler.is_set = RESET_ERROR_HANDLER;
	arena->error_handler.error_code = NO_ERROR;

	if (set_jump_point(&arena->error_handler) == NO_ERROR) {
		return arena_allocate_with_alignment_(arena, size, align, count,
		    flags);
	} else {
		if (arena->error_handler.error_code ==
		    MEMORY_ALLOCATION_ERROR) {
			(void)fprintf(stderr,
			    "Failed to allocate memory from the arena.\n");
		} else {
			(void)fprintf(stderr,
			    "Failed to allocate memory due to another error.\n");
		}
		abort();
	}
}

arena_t
arena_initialize_subarena(arena_t *base, unsigned long cap)
{
	arena_t result = { 0 };
	result.beg = arena_allocate_with_alignment(base, 1, 1, cap, 0);
	result.end = result.beg + cap;
	return result;
}

static void
arena_destroy_(arena_t *arena)
{
	ul_t sz = 0;

	if (!arena || !arena->beg) {
		return;
	}

	sz = (ul_t)(arena->end - arena->beg);

	if (munmap(arena->original, sz) == -1) {

		jump_back(&arena->error_handler, 4);
	}

	arena->beg = NULL;
	arena->end = NULL;
}

void
arena_destroy(arena_t *arena)
{
	if (set_jump_point(&arena->error_handler) == NO_ERROR) {
		arena_destroy_(arena);
	} else {
		if (arena->error_handler.error_code ==
		    INTERNAL_ALLOCATOR_DESTRUCTION_ERROR) {
			(void)fprintf(stderr,
			    "Failed to deallocate the allocated memory from the arena.\n");
		} else {
			(void)fprintf(stderr,
			    "Failed to deallocate arena due to another error.\n");
		}
		abort();
	}
}
