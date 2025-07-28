#pragma once

#include <stdbool.h>
#include <swap_lib_calls.h>

#define MAX_SWAP_TOKEN_LENGTH 15

bool copy_transaction_parameters(create_transaction_parameters_t *sign_transaction_params);

bool validate_swap_amount(uint64_t amount);

bool swap_check_validity();

void __attribute__((noreturn)) finalize_exchange_sign_transaction(bool is_success);
