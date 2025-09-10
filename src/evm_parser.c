#include "evm_parser.h"

#include <string.h>

// Validate that calldata contains selector + two 32-byte words
static bool evm_min_len_ok(size_t len) {
    // 4 bytes selector + 2 * 32 bytes params
    return len >= (EVM_SELECTOR_SIZE + 2 * EVM_WORD_SIZE);
}

// Copy the last 20 bytes of a 32-byte ABI-encoded address word
static void evm_address_from_word(const uint8_t *word32, evm_address_t *addr) {
    // ABI encodes address right-aligned in 32 bytes (12 leading zero bytes)
    memmove(addr->bytes, word32 + (EVM_WORD_SIZE - sizeof(addr->bytes)), sizeof(addr->bytes));
}

// Copy a full 32-byte word as big-endian into raw uint256
static void uint256_from_word(const uint8_t *word32, uint256_raw_t *out) {
    memmove(out->bytes, word32, sizeof(out->bytes));
}

bool parse_transfer_function(const uint8_t *calldata,
                             size_t calldata_len,
                             transfer_calldata_t *out) {
    if (calldata == NULL || out == NULL) return false;
    if (!evm_min_len_ok(calldata_len)) return false;

    // Selector check
    uint32_t selector = ((uint32_t)calldata[0] << 24) | ((uint32_t)calldata[1] << 16) |
                        ((uint32_t)calldata[2] << 8) | (uint32_t)calldata[3];
    if (selector != EVM_ERC20_TRANSFER_SELECTOR) return false;

    const uint8_t *arg0 = calldata + EVM_SELECTOR_SIZE;               // address word
    const uint8_t *arg1 = arg0 + EVM_WORD_SIZE;                        // uint256 amount

    evm_address_from_word(arg0, &out->to);
    uint256_from_word(arg1, &out->amount);
    return true;
}

static void hex_from_bytes(const uint8_t *in, size_t in_len, char *out) {
    static const char HEX[] = "0123456789abcdef";
    for (size_t i = 0; i < in_len; i++) {
        uint8_t b = in[i];
        out[2 * i] = HEX[b >> 4];
        out[2 * i + 1] = HEX[b & 0x0F];
    }
}

bool evm_addr_to_str(const evm_address_t *addr, char *out, size_t out_len) {
    if (addr == NULL || out == NULL) return false;
    if (out_len < EVM_ADDRESS_STR_SIZE) return false;
    out[0] = '0';
    out[1] = 'x';
    hex_from_bytes(addr->bytes, sizeof(addr->bytes), out + 2);
    out[2 + 40] = '\0';
    return true;
}

bool evm_word_to_str(const uint8_t *word32, char *out, size_t out_len) {
    if (word32 == NULL || out == NULL) return false;
    if (out_len < EVM_WORD_STR_SIZE) return false;
    out[0] = '0';
    out[1] = 'x';
    hex_from_bytes(word32, 32, out + 2);
    out[2 + 64] = '\0';
    return true;
}


