#include "sign_contract_call.h"
#include "printf.h"
#include "app_io.h"
#include <string.h>

// Define the supported function selectors
const uint32_t SUPPORTED_FUNCTION_SELECTORS[] = {
    ERC20_TRANSFER_FUNCTION_SELECTOR,
};
const uint32_t SUPPORTED_FUNCTION_SELECTORS_COUNT = sizeof(SUPPORTED_FUNCTION_SELECTORS) / sizeof(SUPPORTED_FUNCTION_SELECTORS[0]);

// // lightweight helpers to print 64-bit values without %lld support
// static void print_int64_field(const char* label, int64_t value) {
//     uint32_t hi = (uint32_t)((uint64_t)value >> 32);
//     uint32_t lo = (uint32_t)((uint64_t)value & 0xFFFFFFFFu);
//     if (hi == 0) {
//         PRINTF("%s%u\n", label, (unsigned)lo);
//     } else {
//         PRINTF("%s0x%08x%08x\n", label, (unsigned)hi, (unsigned)lo);
//     }
// }

static bool is_supported_function(uint8_t* evm_transaction_data) {
    uint32_t function_selector = U4BE(evm_transaction_data, 0);
    for (size_t i = 0; i < SUPPORTED_FUNCTION_SELECTORS_COUNT; i++) {
        if (function_selector == SUPPORTED_FUNCTION_SELECTORS[i]) {
            return true;
        }
    }
    return false;
}

bool parse_and_validate_contract_call(Hedera_ContractCallTransactionBody* contract_call_tx) {
    if (!is_supported_function(contract_call_tx->functionParameters.bytes)) {
        PRINTF("Clear signing for EVM call with function selector 0x%08x is not supported\n", U4BE(contract_call_tx->functionParameters.bytes, 0));
        return false;
    }

    if (contract_call_tx->functionParameters.size > MAX_CONTRACT_CALL_TX_SIZE) {
        PRINTF("Function parameters exceed the limit\n");
        return false;
    }
    //TODO: Parse and validate contract call
    return true;
}

void reformat_contract_call() {
    // TODO: Implement
}

// Contract call handler function
void handle_contract_call_body() {

    // Decode the Contract Call Transaction Body
    Hedera_ContractCallTransactionBody contract_call_tx = st_ctx.transaction.data.contractCall;

    // Verify fields and extract fields to UI global context
    if (!parse_and_validate_contract_call(&contract_call_tx)) {
        THROW(EXCEPTION_MALFORMED_APDU);
    }

    // We jump right into rendering UI and then from button / touch handler we will send stored signature.
    reformat_contract_call();
}