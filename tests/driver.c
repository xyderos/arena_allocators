#include <sys/mman.h>

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "test_hash_trie_allocator/test_allocations.h"
#include "test_linear_arena_allocator/test_allocations.h"
#include "test_probing_allocator/test_allocations.h"

int
main(void)
{
	arena_allocations_suite();
	hash_trie_allocations_suite();
	probing_allocations_suite();
}
