#pragma once

#include <stdint.h>

#include "os.h"

extern void debug_init_stack_canary();

extern uint32_t debug_get_stack_canary();

extern void debug_check_stack_canary();
