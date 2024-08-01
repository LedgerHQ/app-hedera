#pragma once

#include <os.h>
#include <stdint.h>
#include "app_globals.h"

#define MEMCLEAR(element) explicit_bzero(&element, sizeof(element))

#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))

void public_key_to_bytes(unsigned char *dst, uint8_t raw_pubkey[static RAW_PUBKEY_SIZE]);

void bin2hex(uint8_t *dst, uint8_t *data, uint64_t inlen);
