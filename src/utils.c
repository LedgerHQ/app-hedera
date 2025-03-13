#include "utils.h"

/**
 * @brief Converts a raw public key into a specific byte array format.
 *
 * This function takes a raw public key and rearranges its bytes into a
 * destination array. It also sets the highest bit of the last byte based
 * on the parity of a specific byte in the raw public key.
 *
 * @param dst Pointer to the destination byte array where the formatted
 *            public key will be stored.
 * @param raw_pubkey Pointer to the raw public key byte array of size
 *                   RAW_PUBKEY_SIZE.
 *
 * @throws EXCEPTION_MALFORMED_APDU if either dst or raw_pubkey is NULL.
 */
void public_key_to_bytes(unsigned char *dst, uint8_t raw_pubkey[static RAW_PUBKEY_SIZE]) {
    if (dst == NULL || raw_pubkey == NULL) {
        THROW(EXCEPTION_MALFORMED_APDU);
    }

    for (int i = 0; i < 32; i++) {
        dst[i] = raw_pubkey[64 - i];
    }

    if (raw_pubkey[32] & 1) {
        dst[31] |= 0x80;
    }
}

/**
 * @brief Converts binary data to its hexadecimal string representation.
 *
 * This function takes binary data and converts each byte into its
 * corresponding two-character hexadecimal representation. The result is
 * stored in the destination array as a null-terminated string.
 *
 * @param dst Pointer to the destination array where the hexadecimal
 *            string will be stored. It should be large enough to hold
 *            (2 * inlen) + 1 bytes.
 * @param data Pointer to the input binary data array.
 * @param inlen Length of the input binary data array.
 */
void bin2hex(uint8_t *dst, uint8_t *data, uint64_t inlen) {
    static uint8_t const hex[] = "0123456789abcdef";
    for (uint64_t i = 0; i < inlen; i++) {
        dst[2 * i + 0] = hex[(data[i] >> 4) & 0x0F];
        dst[2 * i + 1] = hex[(data[i] >> 0) & 0x0F];
    }
    dst[2 * inlen] = '\0';
}
