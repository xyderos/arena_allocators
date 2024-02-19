#pragma once

#include <check.h>
#include <pthread.h>
#include <stdio.h>

#include "../../src/allocators/probing_map/probing_map.h"
#include "../test_utils/test_utils.h"

void probing_allocations_suite(void);
