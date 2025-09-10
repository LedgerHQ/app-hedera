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
void handle_contract_call_body();
