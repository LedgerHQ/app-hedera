#pragma once

#include "swap_lib_calls.h"

typedef struct hedera_libargs_s {
    unsigned int id;
    unsigned int command;
    unsigned int unused;
    union {
        check_address_parameters_t *check_address;
        create_transaction_parameters_t *create_transaction;
        get_printable_amount_parameters_t *get_printable_amount;
    };
} hedera_libargs_t;