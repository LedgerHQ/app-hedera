#include "hedera.h"

#include <cx.h>
#include <os.h>
#include <string.h>

#include "crypto_helpers.h"
#include "utils.h"

/**
 * @brief Sets the derivation path for the Hedera Ledger application.
 *
 * @param index Index value (not used in this function but kept for consistency).
 * @param path Output array to store the 5-level BIP32 derivation path.
 */
static void hedera_set_path(uint32_t index, uint32_t path[static 5]) {
    path[0] = PATH_ZERO;
    path[1] = PATH_ONE;
    path[2] = PATH_TWO;
    path[3] = PATH_THREE;
    path[4] = PATH_FOUR;
}

/**
 * @brief Derives the public key for a given index and stores it in raw_pubkey.
 *
 * @param index The derivation index for the key.
 * @param raw_pubkey Output buffer to store the derived raw public key (must be at least RAW_PUBKEY_SIZE bytes).
 * @return true if the public key was successfully derived, false otherwise.
 */
bool hedera_get_pubkey(uint32_t index, uint8_t raw_pubkey[static RAW_PUBKEY_SIZE], uint8_t p2) {
    static uint32_t path[5];

    hedera_set_path(index, path);

    int derivation_mode = HDW_ED25519_SLIP10;
    int curve_type = CX_CURVE_Ed25519;

    if (p2 == 1) {
        derivation_mode = HDW_NORMAL;
        curve_type = CX_CURVE_SECP256K1;
    }

    if (CX_OK != bip32_derive_with_seed_get_pubkey_256(
                     derivation_mode, curve_type, path, 5, raw_pubkey,
                     NULL, CX_SHA512, NULL, 0)) {
        return false;
    }

    return true;
}

/**
 * @brief Signs a transaction using the Ed25519 private key derived from the given index.
 *
 * @param index The derivation index for the private key.
 * @param tx Pointer to the transaction data to be signed.
 * @param tx_len Length of the transaction data.
 * @param result Output buffer to store the generated signature (must be at least 64 bytes).
 * @return true if the signing operation was successful, false otherwise.
 */
bool hedera_sign(uint32_t index, const uint8_t* tx, uint8_t tx_len,
                 /* out */ uint8_t* result, uint8_t p2) {
    static uint32_t path[5];
    size_t sig_len = 64;
    uint32_t info;

    hedera_set_path(index, path);

    if (p2 == 0) {
        if (CX_OK != bip32_derive_with_seed_eddsa_sign_hash_256(
                        HDW_ED25519_SLIP10, CX_CURVE_Ed25519, path, 5, CX_SHA512,
                        tx,     // hash (really message)
                        tx_len, // hash length (really message length)
                        result, // signature
                        &sig_len, NULL, 0)) {
            return false;
        }

        return true;
    } else if (p2 == 1) {
        if (CX_OK != bip32_derive_ecdsa_sign_hash_256(CX_CURVE_SECP256K1, 
                                                      path, 
                                                      5, 
                                                      CX_RND_RFC6979 | CX_LAST, CX_SHA256, 
                                                      tx,        // hash (really message)
                                                      tx_len,    // hash length (really message length)
                                                      result,    // signature
                                                      &sig_len,
                                                      &info)) {
            if (info & CX_ECCINFO_PARITY_ODD) {
                result[0] |= 0x01;
            }
            return false;
        }

        return true;
    }

    return true;
}
