#pragma once

#define BUFFER 128

#define SOFT_FAIL_ENABLED
#define ZERO_ALLOCATED_MEMORY_ENABLED

#ifdef SOFT_FAIL_ENABLED
#define SOFT_FAIL_FLAGS 0x2
#endif

#ifdef ZERO_ALLOCATED_MEMORY_ENABLED
#define ZERO_ALLOCATED_FLAGS 0x1
#endif

typedef unsigned long ul_t;
typedef unsigned char byte_t;

extern ul_t bits_per_long;
extern ul_t max_long;
extern ul_t bits_per_long_log;
extern ul_t bytes_per_long_log;
extern ul_t bits_per_byte_log;
extern ul_t malloc_quanta;
extern ul_t one_gigabyte;
extern ul_t default_pool_size;
extern ul_t miss_limit;
extern ul_t alignment;
