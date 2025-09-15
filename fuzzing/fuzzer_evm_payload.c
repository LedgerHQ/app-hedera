#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "evm_parser.h"
#include "ui/app_globals.h" // for MAX_UINT256_LENGTH

extern volatile unsigned int g_last_throw; // from mock

// Simple harness: treat input as raw calldata. Try to parse ERC20 transfer
// and stringify both address and amount using formatting helpers.

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (data == NULL || size == 0) return 0;

    transfer_calldata_t out = {0};
    // Try parse. If fails but selector matches, ensure we do not crash.
    (void)parse_transfer_function(data, size, &out);

    char addr_buf[EVM_ADDRESS_STR_SIZE];
    (void)evm_addr_to_str(&out.to, addr_buf, sizeof(addr_buf));

    char amount_buf[MAX_UINT256_LENGTH + 1];
    (void)evm_word_to_amount(out.amount.bytes, amount_buf);

    // Also test hex formatter for robustness
    char word_hex[EVM_WORD_STR_SIZE];
    (void)evm_word_to_str(out.amount.bytes, word_hex);

    // If input looks like a contract call handler would reject, simulate THROW recording
    if (size >= 4) {
        uint32_t selector = ((uint32_t)data[0] << 24) | ((uint32_t)data[1] << 16) |
                            ((uint32_t)data[2] << 8) | (uint32_t)data[3];
        if (selector == EVM_ERC20_TRANSFER_SELECTOR && size != (EVM_SELECTOR_SIZE + 2 * EVM_WORD_SIZE)) {
            // mirror validate_and_reformat_contract_call strict length check
            g_last_throw = EXCEPTION_MALFORMED_APDU;
        }
    }

    return 0;
}


