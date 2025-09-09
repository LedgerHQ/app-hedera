#pragma once

#include <pb.h>
#include <pb_decode.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "handlers.h"
#include "hedera.h"
#include "app_globals.h"
#include "hedera_format.h"
#include "app_io.h"
#include "proto/contract_call.pb.h"
#include "utils.h"

#define ERC20_TRANSFER_FUNCTION_SELECTOR 0xa9059cbbUL // transfer(address,uint256)

extern const uint32_t SUPPORTED_FUNCTION_SELECTORS[];
extern const uint32_t SUPPORTED_FUNCTION_SELECTORS_COUNT;

// Contract call handler function
void handle_sign_contract_call(uint8_t p1, uint8_t p2, uint8_t* buffer,
                              uint16_t len,
                              /* out */ volatile unsigned int* flags,
                              /* out */ volatile unsigned int* tx);
