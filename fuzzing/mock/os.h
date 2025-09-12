#pragma once

#include <stdint.h>
#include <stddef.h>

// Minimal Ledger OS shim for fuzzing builds

// Exceptions and error handling
static inline void THROW(unsigned int exception) { (void)exception; }
static inline void PRINTF(const char* fmt, ...) { (void)fmt; }

// Memory / security helpers
#ifndef HAVE_EXPLICIT_BZERO
#define HAVE_EXPLICIT_BZERO 1
static inline void explicit_bzero(void *s, size_t n) {
    volatile uint8_t *p = (volatile uint8_t *)s;
    while (n--) { *p++ = 0; }
}
#endif

// Random and timing stubs
static inline void os_random(uint8_t *buffer, size_t length) {
    for (size_t i = 0; i < length; i++) buffer[i] = (uint8_t)i;
}

// PIC stubs compatible with nanopb usage
#ifndef PIC
#define PIC(x) (x)
#endif
static inline void* pic(void* ptr) { return ptr; }


