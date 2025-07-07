#pragma once

#include <stdint.h>
#include <stddef.h>
#include "bolos_sdk_mock.h"

// Mock OS task function declarations
void os_task_init(void);
void os_task_exit(int status);
void os_random(uint8_t *buffer, size_t length);
void os_perso_derive_node_bip32(cx_curve_t curve,
                                const uint32_t *path,
                                size_t path_len,
                                uint8_t *private_key,
                                uint8_t *chain_code);
