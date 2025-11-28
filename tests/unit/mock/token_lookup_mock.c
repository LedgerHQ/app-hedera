#include "tokens/cal/token_lookup.h"

#include <stddef.h>

bool token_info_get_by_address(const token_addr_t address,
                               char ticker[MAX_TICKER_LENG],
                               char name[MAX_TOKEN_LEN],
                               uint32_t* decimals) {
    (void)address;
    (void)ticker;
    (void)name;
    (void)decimals;
    return false;
}

bool token_info_get_by_evm_address(const evm_address_t *evm_address,
                                   char ticker[MAX_TICKER_LENG],
                                   char name[MAX_TOKEN_LEN],
                                   uint32_t* decimals) {
    if (evm_address == NULL) return false;
    // Recognize an address filled with 0x44 bytes as a known token in tests
    bool all44 = true;
    for (size_t i = 0; i < EVM_ADDRESS_SIZE; i++) {
        if (evm_address->bytes[i] != 0x44) { all44 = false; break; }
    }
    if (all44) {
        const char *t = "TOK";
        const char *n = "TokenName";
        for (size_t i = 0; i < MAX_TICKER_LENG && t[i]; i++) ticker[i] = t[i];
        ticker[3] = '\0';
        for (size_t i = 0; i < MAX_TOKEN_LEN && n[i]; i++) name[i] = n[i];
        name[9] = '\0';
        *decimals = 4;
        return true;
    }
    return false;
}

size_t token_info_count(void) {
    return 0;
}


