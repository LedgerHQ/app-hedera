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
    (void)evm_address;
    (void)ticker;
    (void)name;
    (void)decimals;
    return false;
}

size_t token_info_count(void) {
    return 0;
}


