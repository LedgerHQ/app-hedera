#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "app_globals.h"
#include "handlers.h"
#include "hedera.h"
#include "app_io.h"
#include "ui_common.h"
#include "utils.h"

typedef struct get_public_key_context_s {
    uint32_t key_index;

    // Lines on the UI Screen
    char ui_approve_l2[RAW_PUBKEY_SIZE*2 +1];

    uint8_t raw_pubkey[RAW_PUBKEY_SIZE];

    // Public Key Compare
    uint8_t display_index;
    uint8_t full_key[RAW_PUBKEY_SIZE*2 +1];
    uint8_t partial_key[DISPLAY_SIZE + 1];
} get_public_key_context_t;

extern get_public_key_context_t gpk_ctx;
