#ifdef HAVE_SWAP

#include "handle_check_address.h"
#include "../get_public_key.h"
#include <ctype.h>
#include <string.h>

static void derive_public_key(const uint8_t *buffer,
                             uint8_t public_key[RAW_PUBKEY_SIZE],
                             uint8_t public_key_str[RAW_PUBKEY_SIZE]) {
    uint32_t index = U4LE(buffer, 0);

    hedera_get_pubkey(index, public_key);

    public_key_to_bytes(G_io_apdu_buffer, public_key);
    bin2hex(public_key_str, G_io_apdu_buffer, KEY_SIZE);
    public_key_str[KEY_SIZE] = '\0';
}

int handle_check_address(const check_address_parameters_t *params) {
    if (params->coin_configuration != NULL || params->coin_configuration_length != 0) {
        PRINTF("No coin_configuration expected\n");
        return 0;
    }

    if (params->address_parameters == NULL) {
        PRINTF("derivation path expected\n");
        return 0;
    }

    if (params->address_to_check == NULL) {
        PRINTF("Address to check expected\n");
        return 0;
    }
    PRINTF("Address to check %s\n", params->address_to_check);

    if (params->extra_id_to_check == NULL) {
        PRINTF("extra_id_to_check expected\n");
        return 0;
    }
    if (params->extra_id_to_check[0] != '\0') {
        PRINTF("extra_id_to_check expected empty, not '%s'\n",
               params->extra_id_to_check);
        return 0;
    }

    uint8_t public_key[RAW_PUBKEY_SIZE];
    uint8_t public_key_str[RAW_PUBKEY_SIZE];
    derive_public_key(params->address_parameters,
                          public_key,
                          public_key_str);

    UNUSED(public_key);

    if (strcmp(params->address_to_check, (char *) public_key_str) != 0) {
        PRINTF("Address %s != %s\n", params->address_to_check, public_key_str);
        return 0;
    }

    PRINTF("Addresses match\n");
    return 1;
}

#endif