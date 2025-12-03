#pragma once

#include <stdint.h>

#ifndef NO_BOLOS_SDK
#include "os.h"
#endif

#ifdef NO_BOLOS_SDK
// NO_BOLOS_SDK is defined in non-device builds (tests/fuzzers) to stub
// out Ledger OS dependencies while keeping compatible interfaces.
static inline void debug_init_stack_canary() {}
static inline uint32_t debug_get_stack_canary(void) { return 0; }
static inline void debug_check_stack_canary() {}
#else
extern void debug_init_stack_canary();
extern uint32_t debug_get_stack_canary();
extern void debug_check_stack_canary();
#endif
