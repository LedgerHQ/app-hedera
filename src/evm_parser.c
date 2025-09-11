#include "evm_parser.h"

#include <string.h>

#include "ui/app_globals.h"

// Validate that calldata contains selector + two 32-byte words
static bool evm_min_len_ok(size_t len) {
    // 4 bytes selector + 2 * 32 bytes params
    return len >= (EVM_SELECTOR_SIZE + 2 * EVM_WORD_SIZE);
}

// Copy the last 20 bytes of a 32-byte ABI-encoded address word
static void evm_address_from_word(const uint8_t *word32, evm_address_t *addr) {
    // ABI encodes address right-aligned in 32 bytes (12 leading zero bytes)
    memmove(addr->bytes, word32 + EVM_ADDRESS_PADDING_SIZE, EVM_ADDRESS_SIZE);
}

// Copy a full 32-byte word as big-endian into raw uint256
static void uint256_from_word(const uint8_t *word32, uint256_raw_t *out) {
    memmove(out->bytes, word32, EVM_WORD_SIZE);
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
    if (in == NULL || out == NULL) return;
    
    for (size_t i = 0; i < in_len; i++) {
        uint8_t b = in[i];
        out[2 * i] = HEX[b >> 4];
        out[2 * i + 1] = HEX[b & 0x0F];
    }
}

bool evm_addr_to_str(const evm_address_t *addr, char *out) {
    if (addr == NULL || out == NULL) return false;
    out[0] = '0';
    out[1] = 'x';
    hex_from_bytes(addr->bytes, EVM_ADDRESS_SIZE, out + 2);
    out[2 + (EVM_ADDRESS_SIZE * 2)] = '\0';
    return true;
}

bool evm_word_to_str(const uint8_t *word32, char *out) {
    if (word32 == NULL || out == NULL) return false;
    out[0] = '0';
    out[1] = 'x';
    hex_from_bytes(word32, EVM_WORD_SIZE, out + 2);
    out[2 + (EVM_WORD_SIZE * 2)] = '\0';
    return true;
}

bool evm_word_to_amount(const uint8_t *word32, char out[MAX_UINT256_LENGTH]) {
    if (word32 == NULL || out == NULL) return false;
    return uint256_to_decimal(word32, EVM_WORD_SIZE, out, MAX_UINT256_LENGTH);
}

// Helper function to check if buffer is all zeros
static bool allzeroes(const void *buf, size_t n) {
    const uint8_t *p = (const uint8_t *)buf;
    for (size_t i = 0; i < n; i++) {
        if (p[i] != 0) return false;
    }
    return true;
}

// Snippet from app-ethereum/ethereum-plugin-sdk/src/common_utils.c
bool uint256_to_decimal(const uint8_t *value, size_t value_len, char *out, size_t out_len) {
    if (value_len > EVM_WORD_SIZE) {
        // value len is bigger than EVM_WORD_SIZE ?!
        return false;
    }

    uint16_t n[16] = {0};
    // Copy and right-align the number
    memcpy((uint8_t *) n + EVM_WORD_SIZE - value_len, value, value_len);

    // Special case when value is 0
    if (allzeroes(n, EVM_WORD_SIZE)) {
        if (out_len < 2) {
            // Not enough space to hold "0" and \0.
            return false;
        }
        strlcpy(out, "0", out_len);
        return true;
    }

    uint16_t *p = n;
    for (int i = 0; i < 16; i++) {
        n[i] = __builtin_bswap16(*p++);
    }
    int pos = out_len;
    while (!allzeroes(n, sizeof(n))) {
        if (pos == 0) {
            return false;
        }
        pos -= 1;
        unsigned int carry = 0;
        for (int i = 0; i < 16; i++) {
            int rem = ((carry << 16) | n[i]) % 10;
            n[i] = ((carry << 16) | n[i]) / 10;
            carry = rem;
        }
        out[pos] = '0' + carry;
    }
    memmove(out, out + pos, out_len - pos);
    out[out_len - pos] = 0;
    return true;
}