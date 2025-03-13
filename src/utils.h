#pragma once

#include <os.h>
#include <stdint.h>
#include "app_globals.h"

/**
 * @brief Clears memory of a variable.
 *
 * @param element The variable to clear.
 */
#define MEMCLEAR(element) explicit_bzero(&element, sizeof(element))

/**
 * @brief Gets the number of elements in an array.
 *
 * @param array The array to evaluate.
 * @return Number of elements in the array.
 */
#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))

/**
 * @brief Converts a raw public key into a byte array.
 *
 * @param dst Destination byte array.
 * @param raw_pubkey Raw public key input (RAW_PUBKEY_SIZE bytes).
 */
void public_key_to_bytes(unsigned char *dst, uint8_t raw_pubkey[static RAW_PUBKEY_SIZE]);

/**
 * @brief Converts binary data to a hex string.
 *
 * @param dst Destination buffer for hex string.
 * @param data Input binary data.
 * @param inlen Length of input data.
 */
void bin2hex(uint8_t *dst, uint8_t *data, uint64_t inlen);
