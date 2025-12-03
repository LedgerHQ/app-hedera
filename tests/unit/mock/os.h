#pragma once

#include <stdint.h>
#include <stddef.h>

// Minimal Ledger OS shim for unit tests

#ifndef PIC
#define PIC(x) (x)
#endif

// THROW is implemented in tests' throw_mock.c to record last exception
void THROW(unsigned int exception);
static inline void PRINTF(const char* fmt, ...) { (void)fmt; }

// Big-endian helpers used in app code
#ifndef U2BE
#define U2BE(buf, off) (((uint16_t)(buf)[(off)] << 8) | (uint16_t)(buf)[(off) + 1])
#endif
#ifndef U4BE
#define U4BE(buf, off) ((U2BE(buf, off) << 16) | (U2BE(buf, off + 2) & 0xFFFF))
#endif


