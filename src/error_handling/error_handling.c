#include <stdio.h>

#include "error_handling.h"

void
jump_back(error_handler_t *env, error_t error_code)
{
	if (error_code == NO_ERROR) {
		error_code = GENERAL_ERROR;
	}

	switch (error_code) {
	case GENERAL_ERROR:
		(void)fprintf(stderr, "General error occurred.\n");
		break;
	case INTERNAL_ALLOCATOR_CREATION_ERROR:
		(void)fprintf(stderr, "Allocator allocation error occured.\n");
		break;
	case INTERNAL_ALLOCATOR_DESTRUCTION_ERROR:
		(void)fprintf(stderr,
		    "Allocator deallocation error occured.\n");
		break;
	case MEMORY_ALLOCATION_ERROR:
		(void)fprintf(stderr, "Memory allocation error occured.\n");
		break;
	case MEMORY_DEALLOCATION_ERROR:
		(void)fprintf(stderr, "Memory deallocation error occured.\n");
		break;
	case NO_ERROR:
		break;
	}

	env->is_set = SET_ERROR_HANDLER;
	env->error_code = error_code;

	setcontext(&env->context);
}
