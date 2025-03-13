#pragma once

#include <stdint.h>

#include "os.h"

/**
 * @brief Initializes the stack canary for detecting stack overflows.
 */
extern void debug_init_stack_canary();

/**
 * @brief Retrieves the current stack canary value.
 *
 * @return The stack canary value.
 */
extern uint32_t debug_get_stack_canary();

/**
 * @brief Checks if the stack canary has been corrupted.
 */
extern void debug_check_stack_canary();
