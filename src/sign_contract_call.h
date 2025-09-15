#pragma once

#include <pb.h>
#include <pb_decode.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "app_globals.h"

// NO_BOLOS_SDK: when defined, tests/fuzzers exclude BOLOS-only headers
#ifndef NO_BOLOS_SDK
#include "handlers.h"
#include "hedera.h"
#include "hedera_format.h"
#include "app_io.h"
#include "utils.h"
#endif

#include "sign_transaction.h"
#include "proto/contract_call.pb.h"
#include "evm_parser.h"


// Contract call handler function
void handle_contract_call_body();

// Expose validator for fuzz harness
bool validate_and_reformat_contract_call(Hedera_ContractCallTransactionBody* contract_call_tx);
