#include "get_public_key.h"

get_public_key_context_t gpk_ctx;

static bool get_pk(uint8_t p2) {
    // Derive Key
    if (!hedera_get_pubkey(gpk_ctx.key_index, gpk_ctx.raw_pubkey, p2)) {
        return false;
    }

    if (sizeof(G_io_apdu_buffer) < 32) {
        THROW(EXCEPTION_INTERNAL);
    }

    // Put Key bytes in APDU buffer
    public_key_to_bytes(G_io_apdu_buffer, gpk_ctx.raw_pubkey);

    // Populate Key Hex String
    if (p2 == 1) {
        bin2hex(gpk_ctx.full_key, G_io_apdu_buffer, 66);
        gpk_ctx.full_key[66] = '\0';
    } else {
        bin2hex(gpk_ctx.full_key, G_io_apdu_buffer, KEY_SIZE);
        gpk_ctx.full_key[KEY_SIZE] = '\0';
    }


    gpk_ctx.full_key[KEY_SIZE] = '\0';

    return true;
}

void handle_get_public_key(uint8_t p1, uint8_t p2, uint8_t* buffer,
                           uint16_t len,
                           /* out */ volatile unsigned int* flags,
                           /* out */ volatile unsigned int* tx) {
    UNUSED(len);
    UNUSED(tx);

    if (buffer == NULL || len < sizeof(uint32_t)) {
        THROW(EXCEPTION_INTERNAL);
    }

    // Read Key Index
    gpk_ctx.key_index = U4LE(buffer, 0);

    // If p1 != 0, silent mode, for use by apps that request the user's public
    // key frequently Only do UI actions for p1 == 0
    if (p1 == 0) {
        // Complete "Export Public | Key #x?"
        hedera_snprintf(gpk_ctx.ui_approve_l2, DISPLAY_SIZE,
#if defined(TARGET_NANOX) || defined(TARGET_NANOS2) || defined(TARGET_NANOS)
                        "Key #%u?",
#elif defined(TARGET_STAX) || defined(TARGET_FLEX)
                        "#%u",
#endif
                        gpk_ctx.key_index);
    }

    // Populate context with PK
    if (!get_pk(p2)) {
        io_exchange_with_code(EXCEPTION_INTERNAL, 0);
    }

    if (p1 == 0) {
        ui_get_public_key();
    }

    // Normally happens in approve export public key handler
    if (p1 != 0) {
        io_exchange_with_code(EXCEPTION_OK, 32);
        ui_idle();
    }

    *flags |= IO_ASYNCH_REPLY;
}
