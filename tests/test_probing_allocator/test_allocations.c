#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "test_allocations.h"

static void *
probing_map_worker(void *arg)
{
	thread_context_t ctx = *(thread_context_t *)arg;
	arena_object_t value = { 0 }, key = { 0 };
	coordinates_t coord = { 0 }, *res_coord;
	map_slot_t *res_slot = NULL;

	for (ul_t i = ctx.start; i < ctx.stop; i++) {
		coord.x = i;
		coord.y = i * 2;
		value.data = &coord;
		value.size = sizeof(coordinates_t);
		key = probing_map_allocate(ctx.map, value);

		res_slot = probing_map_retrieve(ctx.map, key);
		if (res_slot) {

			res_coord = (coordinates_t *)res_slot->value.data;

			assert(
			    res_coord->x == coord.x && res_coord->y == coord.y);
		} else {
			abort();
		}
		probing_map_deallocate(ctx.map, key);
	}
	return NULL;
}

static void
probing_map_benchmark_template(ul_t capacity, ul_t number_of_threads,
    ul_t chunk_size)
{
	pthread_t threads[number_of_threads];
	thread_context_t *ctx = NULL;
	arena_t perm = arena_initialize(capacity);
	probing_map_t mule;
	mule.capacity = 1000;
	mule.key_counter = 0;
	mule.slots = arena_allocate_with_alignment(&perm,
	    sizeof(map_slot_t) * mule.capacity, _Alignof(map_slot_t), 1, 0);
	mule.arena = perm;
	sem_init(&mule.lock, 0, 1);

	for (ul_t i = 0; i < number_of_threads; i++) {
		ctx = arena_allocate_with_alignment(&perm,
		    sizeof(thread_context_t), _Alignof(thread_context_t), 1, 1);
		ctx->arena = arena_initialize_subarena(&perm, 1 << 23);
		ctx->map = &mule;
		ctx->start = chunk_size * i;
		ctx->stop = chunk_size * (i + 1);
		pthread_create(&threads[i], 0, probing_map_worker, ctx);
	}

	for (ul_t i = 0; i < number_of_threads; i++) {
		pthread_join(threads[i], 0);
	}
	sem_destroy(&mule.lock);
	arena_destroy(&perm);
}

static void
bench_probing_small(void)
{
	static ul_t capacity = (ul_t)1 << 30;
	static ul_t number_of_threads = 10;
	static ul_t chunk_size = 5;
	probing_map_benchmark_template(capacity, number_of_threads, chunk_size);
}

static void
bench_probing_medium(void)
{
	static ul_t capacity = (ul_t)1 << 30;
	static ul_t number_of_threads = 20;
	static ul_t chunk_size = 10;
	probing_map_benchmark_template(capacity, number_of_threads, chunk_size);
}

static void
bench_probing_hard(void)
{
	static ul_t capacity = (ul_t)1 << 30;
	static ul_t number_of_threads = 40;
	static ul_t chunk_size = 15;
	probing_map_benchmark_template(capacity, number_of_threads, chunk_size);
}

void
probing_allocations_suite(void)
{
	bench_probing_small();
	bench_probing_medium();
	bench_probing_hard();
}
