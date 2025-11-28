#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// EVM ABI constants
#define EVM_SELECTOR_SIZE 4
#define EVM_WORD_SIZE 32
#define EVM_ADDRESS_SIZE 20
#define EVM_ADDRESS_PADDING_SIZE 12  // 32 - 20 bytes

// ERC-20 function selectors
#define EVM_ERC20_TRANSFER_SELECTOR 0xa9059cbbUL  // transfer(address,uint256)

// Buffer size constants
#define EVM_ADDRESS_STR_SIZE (2 + 40 + 1)  // "0x" + 40 hex chars + null terminator
#define EVM_WORD_STR_SIZE    (2 + 64 + 1)  // "0x" + 64 hex chars + null terminator

typedef struct evm_address_s {
    // 20-byte EVM address (big-endian)
    uint8_t bytes[EVM_ADDRESS_SIZE];
} evm_address_t;

typedef struct uint256_s {
    // Raw big-endian 32-byte value
    uint8_t bytes[EVM_WORD_SIZE];
} uint256_raw_t;

typedef struct transfer_calldata_s {
    evm_address_t to;
    uint256_raw_t amount;
} transfer_calldata_t;

// Parse ERC-20 transfer(address,uint256) calldata.
// Expects buffer beginning at selector position (offset 0..3) followed by arguments.
// Returns true on success and fills out parameter; false on malformed input.
bool parse_transfer_function(const uint8_t *calldata,
                             size_t calldata_len,
                             transfer_calldata_t *out);

// Recommended buffer sizes for hex strings (including 0x prefix and null-terminator)
// These are now defined above with the other constants

// Convert a 20-byte EVM address to a 0x-prefixed lowercase hex string.
// Caller MUST provide out_len >= EVM_ADDRESS_STR_SIZE. Returns true on success.
bool evm_addr_to_str(const evm_address_t *addr, char *out, size_t out_len);

// Convert a 32-byte ABI word to a decimal string. eg. "1000000000000000000"
// Returns true on success.
bool evm_word_to_amount(const uint8_t *word32, char *out);

// Convert a 32-byte ABI word to a 0x-prefixed lowercase hex string.
// Returns true on success.
bool evm_word_to_str(const uint8_t *word32, char *out);

/**
 * @brief Converts a uint256 value to its decimal string representation.
 *
 * This function takes a uint256 value represented as a byte array, converts it
 * to a decimal string, and stores the result in the provided `out` buffer.
 * Leading zeros in the resulting decimal string are removed.
 *
 * @param value A pointer to the byte array representing the uint256 value.
 * @param value_len The length of the byte array `value` (must be <= 32).
 * @param out A pointer to the buffer where the decimal string representation
 *            will be stored.
 * @param out_len The length of the output buffer `out` (must be >= 2 for "0").
 * @return true on success, false on failure (invalid input or insufficient buffer).
 * 
 * @note Adapted from Ethereum app common_utils.h
 * @see https://github.com/LedgerHQ/ethereum-plugin-sdk/blob/dda423015f2edfdabae9a0eb105fe0a41fe04d97/src/common_utils.h#L110
 * @note Maximum decimal length for uint256 is 78 digits (2^256-1)
 * @note The function performs endianness conversion internally for division operations
 */
bool uint256_to_decimal(const uint8_t *value,
                        size_t value_len,
                        char *out,
                        size_t out_len);

// Format a decimal string representing a uint256 by applying token decimals.
// Example: raw="123456", decimals=4 -> "12.3456"; raw="123", decimals=5 -> "0.00123"
// Trims trailing zeros and a trailing decimal point. Returns true on success.
bool evm_amount_apply_decimals(const char *raw,
                               uint32_t decimals,
                               char *out,
                               size_t out_len);

// ETH-style helpers (ported)
// Imported and adapted from Ledger Ethereum app (Apache-2.0)
// Source: ethereum-plugin-sdk/src/common_utils.c
// Repo: https://github.com/LedgerHQ/ethereum-plugin-sdk
bool adjust_decimals(const char *src,
                    size_t srcLength,
                    char *target,
                    size_t targetLength,
                    uint8_t decimals);

// Imported and adapted from Ledger Ethereum app (Apache-2.0)
// Source: ethereum-plugin-sdk/src/common_utils.c
// Repo: https://github.com/LedgerHQ/ethereum-plugin-sdk
bool evm_amount_to_string(const uint8_t *amount,
                    uint8_t amount_size,
                    uint8_t decimals,
                    const char *ticker,
                    char *out_buffer,
                    size_t out_buffer_size);


