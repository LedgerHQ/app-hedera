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

void handle_contract_call_body() {
    // TODO: Implement
    io_exchange_with_code(EXCEPTION_OK, SIGNATURE_SIZE);
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


// Contract call handler function
void handle_sign_contract_call(uint8_t p1, uint8_t p2, uint8_t* buffer,
                              uint16_t len,
                              /* out */ volatile unsigned int* flags,
                              /* out */ volatile unsigned int* tx) {
    UNUSED(p1);
    UNUSED(p2);
    UNUSED(tx);

    // Raw contract call data
    uint8_t raw_contract_call[MAX_CONTRACT_CALL_TX_SIZE];

    // Validate input
    if (buffer == NULL || len < 4) {
        THROW(EXCEPTION_MALFORMED_APDU);
    }

    // Validate input length (HERE)
    int raw_contract_call_length = (int)len - 4;
    if (raw_contract_call_length > MAX_CONTRACT_CALL_TX_SIZE) {
        THROW(EXCEPTION_MALFORMED_APDU);
    }

    // Key Index (Little Endian format)
    uint32_t key_index = U4LE(buffer, 0);

    // Copy raw contract call data
    memmove(raw_contract_call, (buffer + 4), raw_contract_call_length);

    // Note: Field length validation is enforced before decoding by nanopb.
    // The functionParameters field has a maximum size of 1KB (1024 bytes) as defined
    // in the protobuf schema, preventing oversized function parameters from being processed.

    // Make the in-memory buffer into a stream
    pb_istream_t stream =
        pb_istream_from_buffer(raw_contract_call, raw_contract_call_length);

    // Decode the Contract Call Transaction Body
    Hedera_ContractCallTransactionBody contract_call_tx =
        Hedera_ContractCallTransactionBody_init_zero;
    if (!pb_decode(&stream, Hedera_ContractCallTransactionBody_fields,
                   &contract_call_tx)) {
        PRINTF("Failed to decode contract call protobuf\n");
        THROW(EXCEPTION_MALFORMED_APDU);
    }

    // Verify fields and extract fields to UI global context
    if (!parse_and_validate_contract_call(&contract_call_tx)) {
        THROW(EXCEPTION_MALFORMED_APDU);
    }

    // Generate signature for the contract call and store it in io buffer (Ready to send)
    size_t signature_length = 0;
    if (!hedera_sign(key_index, raw_contract_call, raw_contract_call_length,
                     G_io_apdu_buffer, &signature_length)) {
        THROW(EXCEPTION_MALFORMED_APDU);
    }

    // We jump right into rendering UI and then from button / touch handler we will send stored signature.
    handle_contract_call_body();
}