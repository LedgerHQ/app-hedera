#include <utils.h>

#include "app_globals.h"
#include "glyphs.h"
#include "handlers.h"
#include "os.h"
#include "ui_common.h"
#include "ux.h"
#ifdef HAVE_SWAP
#include "handle_check_address.h"
#include "handle_get_printable_amount.h"
#include "handle_swap_sign_transaction.h"
#include "hedera_swap_utils.h"
#endif

// This is the main loop that reads and writes APDUs. It receives request
// APDUs from the computer, looks up the corresponding command handler, and
// calls it on the APDU payload. Then it loops around and calls io_exchange
// again. The handler may set the 'flags' and 'tx' variables, which affect the
// subsequent io_exchange call. The handler may also throw an exception, which
// will be caught, converted to an error code, appended to the response APDU,
// and sent in the next io_exchange call.

// Things are marked volatile throughout the app to prevent unintended compiler
// reording of instructions (since the try-catch system is a macro)

#if !defined(TARGET_NANOS)
ux_state_t G_ux;
bolos_ux_params_t G_ux_params;
#endif

volatile bool G_called_from_swap;
volatile bool G_swap_response_ready;

static void reset_main_globals(void) {
    MEMCLEAR(G_io_apdu_buffer);
    MEMCLEAR(G_io_seproxyhal_spi_buffer);
}

void app_main() {
    volatile unsigned int rx = 0;
    volatile unsigned int tx = 0;
    volatile unsigned int flags = 0;

    reset_main_globals();

    for (;;) {
        volatile unsigned short sw = 0;

        BEGIN_TRY {
            TRY {
                rx = tx;
                tx = 0; // ensure no race in catch_other if io_exchange throws
                        // an error
                rx = io_exchange(CHANNEL_APDU | flags, rx);
                flags = 0;

                // no APDU received; trigger a reset
                if (rx == 0) {
                    THROW(EXCEPTION_IO_RESET);
                }

                // malformed APDU
                if (G_io_apdu_buffer[OFFSET_CLA] != CLA) {
                    THROW(EXCEPTION_MALFORMED_APDU);
                }

                // APDU handler functions defined in handlers
                switch (G_io_apdu_buffer[OFFSET_INS]) {
                    case INS_GET_APP_CONFIGURATION:
                        // handlers -> get_app_configuration
                        handle_get_app_configuration(
                            G_io_apdu_buffer[OFFSET_P1],
                            G_io_apdu_buffer[OFFSET_P2],
                            G_io_apdu_buffer + OFFSET_CDATA,
                            G_io_apdu_buffer[OFFSET_LC], &flags, &tx);
                        break;

                    case INS_GET_PUBLIC_KEY:
                        // handlers -> get_public_key
                        handle_get_public_key(G_io_apdu_buffer[OFFSET_P1],
                                              G_io_apdu_buffer[OFFSET_P2],
                                              G_io_apdu_buffer + OFFSET_CDATA,
                                              G_io_apdu_buffer[OFFSET_LC],
                                              &flags, &tx);
                        break;

                    case INS_SIGN_TRANSACTION:
                        // handlers -> sign_transaction
                        handle_sign_transaction(G_io_apdu_buffer[OFFSET_P1],
                                                G_io_apdu_buffer[OFFSET_P2],
                                                G_io_apdu_buffer + OFFSET_CDATA,
                                                G_io_apdu_buffer[OFFSET_LC],
                                                &flags, &tx);
                        break;

                    default:
                        THROW(EXCEPTION_UNKNOWN_INS);
                }
            }
            CATCH(EXCEPTION_IO_RESET) { THROW(EXCEPTION_IO_RESET); }
            CATCH_OTHER(e) {
                // Convert exception to response code and add to APDU return
                switch (e & 0xF000) {
                    case 0x6000:
                    case 0x9000:
                        sw = e;
                        break;

                    default:
                        sw = 0x6800 | (e & 0x7FF);
                        break;
                }

                G_io_apdu_buffer[tx++] = sw >> 8;
                G_io_apdu_buffer[tx++] = sw & 0xff;
            }
            FINALLY {
                // explicitly do nothing
            }
        }
        END_TRY;
    }
}

void app_exit(void) {
    // All os calls must be wrapped in a try catch context
    BEGIN_TRY_L(exit) {
        TRY_L(exit) { os_sched_exit(-1); }
        FINALLY_L(exit) {
            // explicitly do nothing
        }
    }
    END_TRY_L(exit);
}

static void start_app_from_lib(void) {
    G_called_from_swap = true;
    G_swap_response_ready = false;
    UX_INIT();
    io_seproxyhal_init();
    USB_power(0);
    USB_power(1);
#ifdef HAVE_BLE
    // Erase globals that may inherit values from exchange
    MEMCLEAR(G_io_asynch_ux_callback);
    // grab the current plane mode setting
    G_io_app.plane_mode = os_setting_get(OS_SETTING_PLANEMODE, NULL, 0);
    BLE_power(0, NULL);
    BLE_power(1, NULL);
#endif  // HAVE_BLE
    app_main();
}

static void library_main_helper(hedera_libargs_t *args) {
    switch (args->command) {
        case CHECK_ADDRESS:
            // ensure result is zero if an exception is thrown
            args->check_address->result = 0;
            args->check_address->result = handle_check_address(args->check_address);
            break;
        case SIGN_TRANSACTION:
            if (copy_transaction_parameters(args->create_transaction)) {
                // never returns
                start_app_from_lib();
            }
        break;
        case GET_PRINTABLE_AMOUNT:
            swap_handle_get_printable_amount(args->get_printable_amount);
            break;
        default:
            break;
    }
}

static void library_main(hedera_libargs_t *args) {
    volatile bool end = false;
    /* This loop ensures that library_main_helper and os_lib_end are called
     * within a try context, even if an exception is thrown */
    while (1) {
        BEGIN_TRY {
            TRY {
                if (!end) {
                    library_main_helper(args);
                }
                os_lib_end();
            }
            FINALLY {
                end = true;
            }
        }
        END_TRY;
    }
}

__attribute__((section(".boot"))) int main(int arg0) {
    // exit critical section (ledger magic)
    __asm volatile("cpsie i");

    // go with the overflow
    debug_init_stack_canary();

    os_boot();

    if (arg0 == 0) {
        for (;;) {
            // Initialize the UX system
            UX_INIT();

            BEGIN_TRY {
                TRY {
                    // Initialize the hardware abstraction layer (HAL) in
                    // the Ledger SDK
                    io_seproxyhal_init();

#ifdef HAVE_BLE
                    // grab the current plane mode setting
                    G_io_app.plane_mode =
                        os_setting_get(OS_SETTING_PLANEMODE, NULL, 0);
#endif // TARGET_NANOX

#ifdef HAVE_BLE
                    BLE_power(0, NULL);
                    BLE_power(1, NULL);
#endif // HAVE_BLE

                    USB_power(0);
                    USB_power(1);

                    // Shows the main menu
                    ui_idle();

                    // Actual Main Loop
                    app_main();
                }
                CATCH(EXCEPTION_IO_RESET) {
                    // reset IO and UX before continuing
                    continue;
                }
                CATCH_ALL { break; }
                FINALLY {
                    // explicitly do nothing
                }
            }
            END_TRY;
        }
    } else {
        hedera_libargs_t *args = (hedera_libargs_t *) arg0;
        if (args->id == 0x100) {
            library_main(args);
        } else {
            app_exit();
        }
    }

    app_exit();

    return 0;
}
