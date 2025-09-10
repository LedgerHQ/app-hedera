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
#include "hedera_format.h"
#include "evm_parser.h"


// Contract call handler function
void handle_contract_call_body();
