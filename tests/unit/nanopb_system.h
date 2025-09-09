#ifndef NANOPB_SYSTEM_H
#define NANOPB_SYSTEM_H

// System headers for nanopb in unit tests
// This avoids BOLOS SDK dependencies by providing the required system headers

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#ifdef PB_ENABLE_MALLOC
#include <stdlib.h>
#endif

// BOLOS SDK compatibility macros for unit tests
#define PIC(x) (x)  // Program memory access - in unit tests, just return the value
#define os_memmove memmove
#define os_memcpy memcpy
#define os_memset memset
#define os_memcmp memcmp

#endif // NANOPB_SYSTEM_H
