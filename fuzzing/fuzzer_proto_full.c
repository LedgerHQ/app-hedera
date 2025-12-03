#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define PB_SYSTEM_HEADER "nanopb_system.h"
#include <pb_decode.h>

#include "proto/contract_call.pb.h"
#include "sign_contract_call.h"
#include "ui/app_globals.h"

// Provide the global context required by handler
sign_tx_context_t st_ctx;

extern volatile unsigned int g_last_throw; // from mock

// Keep inputs reasonable to avoid long-running decodes
#define MAX_PROTO_INPUT 8192

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (data == NULL || size == 0 || size > MAX_PROTO_INPUT) {
        return 0;
    }

    // Decode ContractCallTransactionBody directly
    Hedera_ContractCallTransactionBody call = Hedera_ContractCallTransactionBody_init_zero;
    pb_istream_t stream = pb_istream_from_buffer(data, size);
    if (pb_decode(&stream, Hedera_ContractCallTransactionBody_fields, &call)) {
        // Path 1: Direct validator without THROW side-effects
        (void)validate_and_reformat_contract_call(&call);

        // Path 2: Full handler with THROW side-effects
        memset(&st_ctx, 0, sizeof(st_ctx));
        st_ctx.transaction.data.contractCall = call;
        g_last_throw = 0;
        handle_contract_call_body();
        (void)g_last_throw;
    }

    return 0;
}


