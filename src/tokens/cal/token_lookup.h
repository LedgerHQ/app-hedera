#ifndef TOKEN_LOOKUP_H
#define TOKEN_LOOKUP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../token_address.h"

#define MAX_TICKER_LENG 16
#define MAX_HEDERA_ADDRESS_LENGTH 64
#define MAX_TOKEN_LEN 32

// Mapping to facilitate identification of Tickers and Token Names via the Coin
// Type id
typedef struct token_info {
    token_addr_t address;
    const char ticker[MAX_TICKER_LENG];
    const char token_name[MAX_TOKEN_LEN];
    uint8_t decimals;
} token_info_t;

/**
 * @brief  Get the token info whose address exactly matches @p address and push
 * out the result.
 * @param  address   Pointer to the token's address structure.
 * @param  ticker    Output parameter: buffer to store the token's ticker (must
 *                   be at least MAX_TICKER_LENG).
 * @param  name      Output parameter: buffer to store the token's name (must be
 *                   at least MAX_TOKEN_LEN).
 * @param  decimals  Output parameter: pointer to a uint32_t to store the
 *                   token's decimals.
 * @return true if the token was found and the values were copied, false
 *         otherwise.
 */
bool token_info_get_by_address(const token_addr_t address,
                               char ticker[MAX_TICKER_LENG],
                               char name[MAX_TOKEN_LEN], uint32_t* decimals);

/**
 * @brief  Get the total number of entries in the token table.
 * @return Number of records in the read-only table.
 */
size_t token_info_count(void);

#endif // TOKEN_LOOKUP_H
