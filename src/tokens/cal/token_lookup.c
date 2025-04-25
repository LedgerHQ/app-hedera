#include "token_lookup.h"

#include <string.h>

static const token_info_t token_info_table[] = {
    {"USDC", "0.0.1154552", "hUSDC", 6}, {"SAUCE", "0.0.731861", "SAUCE", 6},
    {"Dovu", "0.0.3716059", "DOVU", 8},  {"PACK", "0.0.4794920", "PACK", 6},
    {"GIB", "0.0.7893707", "GIB", 8},    {"hBARK", "0.0.5022567", "HBARK", 0},
};

static const size_t token_info_table_size =
    sizeof(token_info_table) / sizeof(token_info_table[0]);

const token_info_t *token_info_get_by_address(const char *address) {
    if (address == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < token_info_table_size; ++i) {
        if (strcmp(token_info_table[i].address, address) == 0) {
            return &token_info_table[i];
        }
    }
    return NULL;
}

bool token_info_exists(const char *address) {
    return token_info_get_by_address(address) != NULL;
}

size_t token_info_count(void) { return token_info_table_size; }
