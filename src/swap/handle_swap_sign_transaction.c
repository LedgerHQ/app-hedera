#ifdef HAVE_SWAP

#include "handle_swap_sign_transaction.h"
#include "os.h"
#include "string.h"
#include "swap.h"
#include "sign_transaction.h"
#include "swap_utils.h"

#include <inttypes.h>
#include <sign_transaction.h>

typedef struct swap_validated_s {
    bool initialized;
    uint64_t amount;
    char recipient[ACCOUNT_ID_SIZE];
    uint64_t fee;
} swap_validated_t;

static swap_validated_t G_swap_validated;

static uint8_t *G_swap_sign_return_value_address;

bool copy_transaction_parameters(create_transaction_parameters_t *params) {
    if (params->coin_configuration != NULL || params->coin_configuration_length != 0) {
        PRINTF("No coin_configuration expected\n");
        return false;
    }

    if (params->destination_address_extra_id == NULL) {
        PRINTF("destination_address_extra_id expected\n");
        return false;
    }
    
    if (params->destination_address_extra_id[0] != '\0') {
        PRINTF("WARNING: destination_address_extra_id expected empty, not '%s'\n",
               params->destination_address_extra_id);
    }

    swap_validated_t swap_validated;
    memset(&swap_validated, 0, sizeof(swap_validated));

    strlcpy(swap_validated.recipient,
            params->destination_address,
            sizeof(swap_validated.recipient));
    if (swap_validated.recipient[sizeof(swap_validated.recipient) - 1] != '\0') {
        PRINTF("Address copy error\n");
        return false;
    }

    if (!swap_str_to_u64(params->amount, params->amount_length, &swap_validated.amount)) {
        return false;
    }
    if (!swap_str_to_u64(params->fee_amount, params->fee_amount_length, &swap_validated.fee)) {
        return false;
    }
    swap_validated.initialized = true;

    os_explicit_zero_BSS_segment();

    G_swap_sign_return_value_address = &params->result;

    memcpy(&G_swap_validated, &swap_validated, sizeof(swap_validated));

    return true;
}

bool validate_swap_amount(uint64_t amount) {

    if (amount != G_swap_validated.amount) {
        PRINTF("Amount not equal\n");
        return false;
    }
    char validated_amount_str[MAX_PRINTABLE_AMOUNT_SIZE];
    if (print_token_amount(G_swap_validated.amount,HEDERA_SIGN,HEDERA_DECIMALS, validated_amount_str, sizeof(validated_amount_str)) != 0) {
        PRINTF("Conversion failed\n");
        return false;
    }
    char amount_str[MAX_PRINTABLE_AMOUNT_SIZE];
    if (print_token_amount(amount, HEDERA_SIGN, HEDERA_DECIMALS, amount_str, sizeof(amount_str)) != 0) {
        PRINTF("Conversion failed\n");
        return false;
    }

    if (strcmp(amount_str, validated_amount_str) != 0) {
        PRINTF("Amount requested in this transaction = %s\n", amount_str);
        PRINTF("Amount validated in swap = %s\n", validated_amount_str);
        return false;
    }
    return true;
}

bool swap_check_validity() {
    if (!G_swap_validated.initialized) {
        PRINTF("Swap Validated data not initialized.\n");
        return false;
    }

    if (!validate_swap_amount(st_ctx.transaction.data.cryptoTransfer.transfers
                        .accountAmounts[0]
                        .amount)) {
        PRINTF("Amount on Transaction is different from validated package.\n");
        return false;
    }

    if (st_ctx.transaction.transactionFee != G_swap_validated.fee) {
        PRINTF("Gas fee on Transaction is different from validated package.\n");
        PRINTF("Gas fee on Transaction: %d\n", (uint32_t) st_ctx.transaction.transactionFee);
        PRINTF("Gas fee on Swap: %d\n", (uint32_t) G_swap_validated.fee);
        return false;
    }

    if (memcmp(st_ctx.recipients, G_swap_validated.recipient, sizeof(st_ctx.recipients)) != 0) {
        PRINTF("Recipient on Transaction is different from validated package.\n");
        PRINTF("Recipient requested in the transaction: %.*H\n", FULL_ADDRESS_LENGTH, st_ctx.recipients);
        PRINTF("Recipient validated in the swap: %.*H\n", FULL_ADDRESS_LENGTH, G_swap_validated.recipient);
        return false;
    }

    return true;
}


void __attribute__((noreturn)) finalize_exchange_sign_transaction(bool is_success) {
    PRINTF("Returning to Exchange with status %d\n", is_success);
    *G_swap_sign_return_value_address = is_success;
    os_lib_end();
}

#endif