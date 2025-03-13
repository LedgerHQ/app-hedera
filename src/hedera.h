#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "app_globals.h"

/**
 * @brief Retrieves the public key for a given index.
 *
 * @param index Index to derive the key from.
 * @param raw_pubkey Buffer to store the derived public key (size: RAW_PUBKEY_SIZE).
 * @return true if successful; false otherwise.
 */
bool hedera_get_pubkey(uint32_t index, uint8_t raw_pubkey[static RAW_PUBKEY_SIZE], uint8_t p2);

/**
 * @brief Signs a transaction using the key derived from the given index.
 *
 * @param index Index to derive the signing key from.
 * @param tx Pointer to the transaction data.
 * @param tx_len Length of the transaction data.
 * @param result Buffer to store the signature.
 * @return true if signing is successful; false otherwise.
 */
bool hedera_sign(uint32_t index, const uint8_t* tx, uint8_t tx_len,
                 /* out */ uint8_t* result, uint8_t p2);
