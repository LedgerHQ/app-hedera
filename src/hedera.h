#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "app_globals.h"
#include <stddef.h>

bool hedera_get_pubkey(uint32_t index, uint8_t raw_pubkey[static RAW_PUBKEY_SIZE]);

bool hedera_sign(uint32_t index, const uint8_t* tx, uint8_t tx_len,
                 /* out */ uint8_t* result, /* out */ size_t* sig_len_out);
