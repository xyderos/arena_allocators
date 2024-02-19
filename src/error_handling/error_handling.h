#pragma once

#include <ucontext.h>

typedef enum { RESET_ERROR_HANDLER, SET_ERROR_HANDLER } set_error_handler_t;

typedef enum {
	NO_ERROR,
	GENERAL_ERROR,
	INTERNAL_ALLOCATOR_CREATION_ERROR,
	INTERNAL_ALLOCATOR_DESTRUCTION_ERROR,
	MEMORY_ALLOCATION_ERROR,
	MEMORY_DEALLOCATION_ERROR
} error_t;

typedef struct error_handler {
	ucontext_t context;
	int is_set;
	error_t error_code;
} error_handler_t;

#define barrier __asm__ __volatile__("" ::: "memory")

#define set_jump_point(_bufp)                          \
	({                                             \
		(_bufp)->is_set = RESET_ERROR_HANDLER; \
		barrier;                               \
		getcontext(&(_bufp)->context);         \
		(_bufp)->is_set;                       \
	})

void jump_back(error_handler_t *, error_t);
