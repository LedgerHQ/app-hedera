#include "token_lookup.h"
#include "cal.h"
#include <string.h>

bool token_info_get_by_address(const token_addr_t address,
                               char ticker[MAX_TICKER_LENG],
                               char name[MAX_TOKEN_LEN], uint32_t* decimals) {
    if ((address.addr_account == 0 && address.addr_realm == 0 &&
        address.addr_shard == 0) || ticker == NULL || name == NULL ||
        decimals == NULL) {
        // Invalid address
        return false;
    }
    for (size_t i = 0; i < token_info_table_size; i++) {
        if (token_info_table[i].address.addr_account == address.addr_account &&
            token_info_table[i].address.addr_realm == address.addr_realm &&
            token_info_table[i].address.addr_shard == address.addr_shard) {
            // Found the token
            strncpy(ticker, token_info_table[i].ticker, MAX_TICKER_LENG);
            strncpy(name, token_info_table[i].token_name, MAX_TOKEN_LEN);
            *decimals = token_info_table[i].decimals;
            return true;
        }
    }
    // Token not found
    return false;
}

size_t token_info_count(void) { return token_info_table_size; }

static bool evm_addr_is_zero(const evm_address_t *addr) {
    for (size_t i = 0; i < EVM_ADDRESS_SIZE; i++) {
        if (addr->bytes[i] != 0) return false;
    }
    return true;
}

static bool evm_addr_equal(const evm_address_t *a, const evm_address_t *b) {
    return memcmp(a->bytes, b->bytes, EVM_ADDRESS_SIZE) == 0;
}

bool token_info_get_by_evm_address(const evm_address_t *evm_address,
                                   char ticker[MAX_TICKER_LENG],
                                   char name[MAX_TOKEN_LEN],
                                   uint32_t* decimals) {
    if (evm_address == NULL || ticker == NULL || name == NULL || decimals == NULL) {
        return false;
    }
    // Reject zero address lookups
    if (evm_addr_is_zero(evm_address)) {
        return false;
    }
    for (size_t i = 0; i < token_info_table_size; i++) {
        if (!evm_addr_is_zero(&token_info_table[i].evm_address) &&
            evm_addr_equal(&token_info_table[i].evm_address, evm_address)) {
            strncpy(ticker, token_info_table[i].ticker, MAX_TICKER_LENG);
            strncpy(name, token_info_table[i].token_name, MAX_TOKEN_LEN);
            *decimals = token_info_table[i].decimals;
            return true;
        }
    }
    return false;
}
