#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include "../../src/arena/arena/arena.h"
#include "../test_utils/test_utils.h"
#include "test_allocations.h"

static ul_t
tostr(unsigned char *buf, int v)
{
	ul_t len = (v < 0) + 1;
	for (int t = v; t /= 10; len++) {
	}
	int t = v < 0 ? v : -v;
	unsigned char *p = buf + len;
	do {
		*--p = '0' - (unsigned char)(t % 10);
	} while (t /= 10);
	if (v < 0) {
		*--p = '-';
	}
	return len;
}

static void *
hash_trie_worker(void *arg)
{
	thread_context_t ctx = *(thread_context_t *)arg;
	arena_object_t key = { 0 }, value = { 0 };
	coordinates_t coord = { 0 }, *res_coord = NULL;
	hash_trie_t *retrieved = NULL;

	for (ul_t i = ctx.start; i < ctx.stop; i++) {
		unsigned char buf[32];
		coord.x = 10 + i;
		coord.y = 20 * i;
		key.data = buf;
		key.size = tostr(buf, (int)i);
		value.data = &coord;
		value.size = sizeof(coordinates_t);

		hash_trie_allocate(ctx.trie, &ctx.arena, key, value);

		retrieved = hash_trie_retrieve(ctx.trie, key);

		if (retrieved) {
			res_coord = (coordinates_t *)retrieved->value.data;
			assert(res_coord && res_coord->x == coord.x &&
			    res_coord->y == coord.y);
		}

		hash_trie_deallocate(ctx.trie, key);
	}

	return NULL;
}

static void
hash_trie_benchmark_template(ul_t capacity, ul_t number_of_threads,
    ul_t chunk_size)
{
	arena_t perm = arena_initialize(capacity);
	thread_context_t *ctx = NULL;
	pthread_t threads[number_of_threads];
	hash_trie_t *mule = NULL;

	for (ul_t i = 0; i < number_of_threads; i++) {
		ctx = arena_allocate_with_alignment(&perm,
		    sizeof(thread_context_t), _Alignof(thread_context_t), 1, 1);
		ctx->arena = arena_initialize_subarena(&perm, 1 << 23);
		ctx->trie = &mule;
		ctx->start = chunk_size * (i + 0);
		ctx->stop = chunk_size * (i + 1);
		pthread_create(threads + i, 0, hash_trie_worker, ctx);
	}
	for (ul_t i = 0; i < number_of_threads; i++) {
		pthread_join(threads[i], 0);
	}

	arena_destroy(&perm);
}

static void
bench_trie_small(void)
{
	static ul_t capacity = (ul_t)1 << 30;
	static ul_t number_of_threads = 32;
	static ul_t chunk_size = 100;
	hash_trie_benchmark_template(capacity, number_of_threads, chunk_size);
}

static void
bench_trie_medium(void)
{
	static ul_t capacity = (ul_t)1 << 30;
	static ul_t number_of_threads = 32;
	static ul_t chunk_size = 1000;
	hash_trie_benchmark_template(capacity, number_of_threads, chunk_size);
}

static void
bench_trie_hard(void)
{
	static ul_t capacity = (ul_t)1 << 30;
	static ul_t number_of_threads = 32;
	static ul_t chunk_size = 10000;
	hash_trie_benchmark_template(capacity, number_of_threads, chunk_size);
}

void
hash_trie_allocations_suite(void)
{
	bench_trie_small();
	bench_trie_medium();
	bench_trie_hard();
}
