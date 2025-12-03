#pragma once

#include <stdint.h>
#include <stddef.h>

// Minimal Ledger OS shim for fuzzing builds

// Exceptions and error handling
extern volatile unsigned int g_last_throw;
static inline void THROW(unsigned int exception) { g_last_throw = exception; }
static inline void PRINTF(const char* fmt, ...) { (void)fmt; }

// PIC stubs compatible with nanopb usage
#ifndef PIC
#define PIC(x) (x)
#endif
static inline void* pic(void* ptr) { return ptr; }

// Big-endian helpers used in app code
#ifndef U2BE
#define U2BE(buf, off) (((uint16_t)(buf)[(off)] << 8) | (uint16_t)(buf)[(off) + 1])
#endif
#ifndef U4BE
#define U4BE(buf, off) ((U2BE(buf, off) << 16) | (U2BE(buf, off + 2) & 0xFFFF))
#endif

// Provide EXCEPTION codes matching unit tests
#ifndef EXCEPTION_MALFORMED_APDU
#define EXCEPTION_MALFORMED_APDU 0x6E00
#endif


