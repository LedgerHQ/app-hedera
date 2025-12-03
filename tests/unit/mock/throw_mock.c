#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

// Expose last thrown exception for assertions
volatile unsigned int g_last_throw = 0;

void THROW(unsigned int exception) {
    g_last_throw = exception;
}


