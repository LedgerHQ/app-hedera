#include "sign_contract_call.h"
#include "evm_parser.h"
#include "printf.h"
#include "app_io.h"
#include <string.h>

// UI/UX Context
sign_tx_context_t st_ctx;

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

bool parse_and_validate_contract_call(Hedera_ContractCallTransactionBody* contract_call_tx) {

    MEMCLEAR(st_ctx.recipients);
    MEMCLEAR(st_ctx.amount);
    MEMCLEAR(st_ctx.senders);
    MEMCLEAR(st_ctx.key_index_str);
    MEMCLEAR(st_ctx.senders_title);
    MEMCLEAR(st_ctx.recipients_title);
    MEMCLEAR(st_ctx.amount_title);
    MEMCLEAR(st_ctx.operator);
    MEMCLEAR(st_ctx.senders);

    reformat_key_index();
    set_senders_title("From");
    st_ctx.senders = "To implement";

    uint32_t function_selector = U4BE(contract_call_tx->functionParameters.bytes, 0);
    
    switch (function_selector) {
        case EVM_ERC20_TRANSFER_SELECTOR: {
            // Parse ERC-20 transfer(address,uint256) parameters
            transfer_calldata_t transfer_data;
            if (parse_transfer_function(contract_call_tx->functionParameters.bytes,
                                       contract_call_tx->functionParameters.size,
                                       &transfer_data)) {

                if (!evm_addr_to_str(&transfer_data.to, st_ctx.recipients, sizeof(st_ctx.recipients))) {
                    PRINTF("Failed to stringify EVM address\n");
                    return false;
                }

                if (!evm_word_to_str(transfer_data.amount.bytes, st_ctx.amount, sizeof(st_ctx.amount))) {
                    PRINTF("Failed to stringify amount word\n");
                    return false;
                }

                set_amount_title("Raw token amount");

            } else {
                PRINTF("Failed to parse ERC-20 transfer parameters\n");
                return false;
            }
            break;
        }
        // To future developers: Add more function selectors here :D
        default:
            PRINTF("Unsupported function selector: %x\n", function<<<<<<< HEAD
                static bool is_supported_function(uint8_t* evm_transaction_data) {
                    uint32_t function_selector = U4BE(evm_transaction_data, 0);
                    for (size_t i = 0; i < SUPPORTED_FUNCTION_SELECTORS_COUNT; i++) {
                        if (function_selector == SUPPORTED_FUNCTION_SELECTORS[i]) {
                            return true;
                        }
                    }
                    return false;
                =======
                void handle_contract_call_body() {
                    
                   
                    io_exchange_with_code(EXCEPTION_OK, SIGNATURE_SIZE);
                >>>>>>> 624aa703 (feat: add EVM decoding)
                }_selector);
            return false;
    }

    hedera_safe_printf(st_ctx.fee, "%llu", contract_call_tx->gas);
    
    
    PRINTF("==========================================\n");
    PRINTF("Formatted fields\n");
    PRINTF("Key Index: %s\n", st_ctx.key_index_str);
    PRINTF("From: %s\n", st_ctx.senders);
    PRINTF("function_selector: %x\n", function_selector);
    PRINTF("Recipients: %s\n", st_ctx.recipients);
    PRINTF("Amount: %s\n", st_ctx.amount);
    PRINTF("Fee: %s\n", st_ctx.fee);
    PRINTF("==========================================\n");
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