#ifndef TOKEN_LOOKUP_H
#define TOKEN_LOOKUP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_TICKER_LENG           16
#define MAX_HEDERA_ADDRESS_LENGTH 64
#define MAX_TOKEN_LEN             32

// Mapping to facilitate identification of Tickers and Token Names via the Coin Type id
typedef struct token_info {
    const char ticker[MAX_TICKER_LENG];
    const char address[MAX_HEDERA_ADDRESS_LENGTH];
    const char token_name[MAX_TOKEN_LEN];
    uint8_t decimals;
} token_info_t;

/**
 * @brief  Find the token_info record whose address exactly matches @p address.
 * @param  address  Null-terminated string of the token’s address.
 * @return Pointer to a const token_info_t in the table, or NULL if not found.
 */
const token_info_t * token_info_get_by_address(const char *address);

/**
 * @brief  Test whether a given token address exists in the table.
 * @param  address  Null-terminated string of the token’s address.
 * @return true if found, false otherwise.
 */
bool token_info_exists(const char *address);

/**
 * @brief  Get the total number of entries in the token table.
 * @return Number of records in the read-only table.
 */
size_t token_info_count(void);

#endif // TOKEN_LOOKUP_H
