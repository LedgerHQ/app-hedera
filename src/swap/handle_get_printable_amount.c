#ifdef HAVE_SWAP

#include "handle_get_printable_amount.h"
#include "hedera_format.h"
#include "swap_utils.h"
#include <utils.h>

int print_amount(uint64_t amount, char *out, size_t out_length) {
    return print_token_amount(amount, HEDERA_SIGN, HEDERA_DECIMALS, out, out_length);
}


int swap_handle_get_printable_amount(get_printable_amount_parameters_t* params) {
    MEMCLEAR(params->printable_amount);

    if (params->coin_configuration != NULL || params->coin_configuration_length != 0) {
        PRINTF("No coin_configuration expected\n");
        return 0;
    }

    uint64_t amount = 0;
    if (!swap_str_to_u64(params->amount, params->amount_length, &amount)) {
        PRINTF("Amount is too big");
        return 0;
    }

    if (print_amount(amount, params->printable_amount, sizeof(params->printable_amount)) != 0) {
        PRINTF("print_amount failed");
        return 0;
    }

    return 1;
}
#endif