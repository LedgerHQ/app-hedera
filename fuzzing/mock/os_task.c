#include "os_task.h"
#include <string.h>

// Mock OS task functions for fuzzing
void os_task_init(void) {
    // No-op for fuzzing
}

void os_task_exit(int status) {
    // In fuzzing, we don't want to actually exit
    (void)status;
}

// Mock random number generation
void os_random(uint8_t *buffer, size_t length) {
    // Simple predictable pattern for fuzzing
    for (size_t i = 0; i < length; i++) {
        buffer[i] = (uint8_t)(i & 0xFF);
    }
}

// Mock secure memory operations
void os_perso_derive_node_bip32(cx_curve_t curve,
                                const uint32_t *path,
                                size_t path_len,
                                uint8_t *private_key,
                                uint8_t *chain_code) {
    (void)curve;
    (void)path;
    (void)path_len;
    
    // Mock derivation - fill with predictable patterns
    if (private_key) {
        memset(private_key, 0x42, 32);
    }
    if (chain_code) {
        memset(chain_code, 0x84, 32);
    }
}
