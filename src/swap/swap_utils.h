#pragma once

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"

#define HEDERA_SIGN "HBAR"
#define HEDERA_DECIMALS 8

extern volatile bool G_called_from_swap;
extern volatile bool G_swap_response_ready;

bool swap_str_to_u64(const uint8_t* src, size_t length, uint64_t* result);

int print_token_amount(uint64_t amount, const char *asset, uint8_t decimals,
                       char *out, size_t out_length);
