#include <stdint.h>

#include "app_io.h"
#include "os.h"
#include "ux.h"

void handle_get_app_configuration(
    uint8_t p1, uint8_t p2, const uint8_t* const buffer, uint16_t len,
    /* out */ volatile const unsigned int* const flags,
    /* out */ volatile const unsigned int* const tx) {
    UNUSED(p1);
    UNUSED(p2);
    UNUSED(buffer);
    UNUSED(len);
    UNUSED(flags);
    UNUSED(tx);

    if (sizeof(G_io_apdu_buffer) < 4) {
        THROW(EXCEPTION_INTERNAL);
    }

    // storage allowed?
    G_io_apdu_buffer[0] = 0;

    // version
    G_io_apdu_buffer[1] = MAJOR_VERSION;
    G_io_apdu_buffer[2] = MINOR_VERSION;
    G_io_apdu_buffer[3] = PATCH_VERSION;

    io_exchange_with_code(EXCEPTION_OK, 4);
}
