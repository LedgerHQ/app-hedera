#include "utils.h"
#include "globals.h"

void public_key_to_bytes(unsigned char *dst, uint8_t raw_pubkey[static 65]) {
    if (dst == NULL || raw_pubkey == NULL) {
        THROW(EXCEPTION_MALFORMED_APDU);
    }
    
    for (int i = 0; i < 32; i++) {
        dst[ i ] = raw_pubkey[ 64 - i ];
    }

    if (raw_pubkey[ 32 ] & 1) {
        dst[ 31 ] |= 0x80;
    }
}

void bin2hex(uint8_t *dst, uint8_t *data, uint64_t inlen) {
    static uint8_t const hex[] = "0123456789abcdef";
    for (uint64_t i = 0; i < inlen; i++) {
        dst[ 2 * i + 0 ] = hex[ (data[ i ] >> 4) & 0x0F ];
        dst[ 2 * i + 1 ] = hex[ (data[ i ] >> 0) & 0x0F ];
    }
    dst[ 2 * inlen ] = '\0';
}
