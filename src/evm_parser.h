#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// EVM ABI constants
#define EVM_SELECTOR_SIZE 4
#define EVM_WORD_SIZE 32

// ERC-20 function selectors
#define EVM_ERC20_TRANSFER_SELECTOR 0xa9059cbbUL  // transfer(address,uint256)

typedef struct evm_address_s {
    // 20-byte EVM address (big-endian)
    uint8_t bytes[20];
} evm_address_t;

typedef struct uint256_s {
    // Raw big-endian 32-byte value
    uint8_t bytes[32];
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
#define EVM_ADDRESS_STR_SIZE (2 + 40 + 1)
#define EVM_WORD_STR_SIZE    (2 + 64 + 1)

// Convert a 20-byte EVM address to a 0x-prefixed lowercase hex string.
// Returns true on success.
bool evm_addr_to_str(const evm_address_t *addr, char *out);

// Convert a 32-byte ABI word to a decimal string. eg. "1000000000000000000"
// Returns true on success.
bool evm_word_to_amount(const uint8_t *word32, char *out);

// Convert a 32-byte ABI word to a 0x-prefixed lowercase hex string.
// Returns true on success.
bool evm_word_to_str(const uint8_t *word32, char *out);

// Convert a uint256 value to its decimal string representation.
// Leading zeros are removed. Returns true on success.
bool uint256_to_decimal(const uint8_t *value, size_t value_len, char *out, size_t out_len);


