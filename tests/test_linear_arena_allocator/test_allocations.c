#include <assert.h>
#include <pthread.h>

#include "../../src/arena/arena/arena.h"
#include "../test_utils/test_utils.h"
#include "test_allocations.h"

static void *
arena_worker(void *arg)
{
	coordinates_t *allocated = NULL;

	thread_context_t ctx = *(thread_context_t *)arg;
	for (ul_t i = ctx.start; i < ctx.stop; i++) {
		allocated = arena_allocate_with_alignment(&ctx.arena,
		    sizeof(coordinates_t), _Alignof(coordinates_t), 1, 1);

		allocated->x = 1;
		allocated->y = 2;
		assert(allocated->x == 1);
		assert(allocated->y == 2);
	}
	return 0;
}

static void
arena_benchmark_template(ul_t capacity, ul_t number_of_threads, ul_t chunk_size)
{
	arena_t perm = arena_initialize(capacity);
	thread_context_t *ctx = NULL;
	pthread_t threads[number_of_threads];

	for (ul_t i = 0; i < number_of_threads; i++) {
		ctx = arena_allocate_with_alignment(&perm,
		    sizeof(thread_context_t), _Alignof(thread_context_t), 1, 1);
		ctx->arena = arena_initialize_subarena(&perm, 1 << 10);
		ctx->start = chunk_size * (i + 0);
		ctx->stop = chunk_size * (i + 1);
		pthread_create(threads + i, 0, arena_worker, ctx);
	}
	for (ul_t i = 0; i < number_of_threads; i++) {
		pthread_join(threads[i], 0);
	}

	arena_destroy(&perm);
}

static void
bench_arena_small(void)
{
	static ul_t capacity = (ul_t)1 << 15;
	static ul_t number_of_threads = 10;
	static ul_t chunk_size = 50;
	arena_benchmark_template(capacity, number_of_threads, chunk_size);
}

static void
bench_arena_medium(void)
{
	static ul_t capacity = (ul_t)1 << 20;
	static ul_t number_of_threads = 20;
	static ul_t chunk_size = 50;
	arena_benchmark_template(capacity, number_of_threads, chunk_size);
}

static void
bench_arena_hard(void)
{
	static ul_t capacity = (ul_t)1 << 30;
	static ul_t number_of_threads = 30;
	static ul_t chunk_size = 50;
	arena_benchmark_template(capacity, number_of_threads, chunk_size);
}

void
arena_allocations_suite(void)
{
	bench_arena_small();
	bench_arena_medium();
	bench_arena_hard();
}
