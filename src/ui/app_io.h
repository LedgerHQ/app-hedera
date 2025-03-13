#pragma once

#include "debug.h"
#include "app_globals.h"
#include "os.h"
#include "os_io_seproxyhal.h"

/**
 * @brief Exchanges APDU data with a status code.
 *
 * @param code Status code to send.
 * @param tx   Length of the response data.
 */
extern void io_exchange_with_code(uint16_t code, uint16_t tx);
