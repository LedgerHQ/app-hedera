#include "hedera.h"

#include <cx.h>
#include <os.h>
#include <string.h>

#include "crypto_helpers.h"
#include "utils.h"

static void hedera_set_path(uint32_t index, uint32_t path[static 5]) {
    path[0] = PATH_ZERO;
    path[1] = PATH_ONE;
    path[2] = PATH_TWO;
    path[3] = PATH_THREE;
    path[4] = PATH_FOUR;
}

bool hedera_get_pubkey(uint32_t index, uint8_t raw_pubkey[static RAW_PUBKEY_SIZE]) {
    static uint32_t path[5];

    hedera_set_path(index, path);

    if (CX_OK != bip32_derive_with_seed_get_pubkey_256(
                     HDW_ED25519_SLIP10, CX_CURVE_Ed25519, path, 5, raw_pubkey,
                     NULL, CX_SHA512, NULL, 0)) {
        return false;
    }

    return true;
}

bool hedera_sign(uint32_t index, const uint8_t* tx, uint8_t tx_len,
                 /* out */ uint8_t* result) {
    static uint32_t path[5];
    size_t sig_len = 64;

    hedera_set_path(index, path);

    if (CX_OK != bip32_derive_with_seed_eddsa_sign_hash_256(
                     HDW_ED25519_SLIP10, CX_CURVE_Ed25519, path, 5, CX_SHA512,
                     tx,     // hash (really message)
                     tx_len, // hash length (really message length)
                     result, // signature
                     &sig_len, NULL, 0)) {
        return false;
    }

    return true;
}
