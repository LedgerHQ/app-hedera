#pragma once

#include <pb.h>
#include <pb_decode.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "crypto_create.pb.h"
#include "crypto_update.pb.h"
#include "handlers.h"
#include "hedera.h"
#include "hedera_format.h"
#include "io.h"
#include "transaction_body.pb.h"
#include "ui_common.h"
#include "utils.h"

enum TransactionStep {
    Summary = 1,
    Operator = 2,
    Senders = 3,
    Recipients = 4,
    Amount = 5,
    Fee = 6,
    Memo = 7,
    Confirm = 8,
    Deny = 9
};

enum TransactionType {
    Unknown = -1,
    Verify = 0,
    Create = 1,
    Update = 2,
    Transfer = 3,
    Associate = 4,
    Dissociate = 5,
    TokenTransfer = 6,
    TokenMint = 7,
    TokenBurn = 8,
};

/*
 * Supported Transactions:
 * Verify:
 * "Verify Account with Key #0?" (Summary) <--> "Account" (Senders) <--> Confirm
 * <--> Deny
 *
 * Create:
 * "Create Account with Key #0?" (Summary) <--> Operator <--> "Stake to"
(Senders)
 * <--> "Collect Rewards? Yes / No" (Recipients) <--> "Initial Balance" (Amount)
 * <--> Fee <--> Memo <--> Confirm <--> Deny
 *
 * Update:
 * "Update Account 0.0.0 with Key #0?" (Summary) <--> Operator <-->
 * "Stake to" (Senders) <--> "Collect Rewards (Yes / No)" (Recipients) <-->
 * "Updated Account" (Amount) <--> Fee <--> Memo <--> Confirm <--> Deny
 *
 * Transfer:
 * "Transfer with Key #0?" (Summary) <--> Operator <--> Senders <--> Recipients
 * <--> Amount <--> Fee <--> Memo <--> Confirm <--> Deny
 *
 * Associate:
 * "Associate Token with Key #0?" (Summary) <--> Operator <--> "Token" (Senders)
 * <--> Fee <--> Memo <--> Confirm <--> Deny
 *
 * Dissociate:
 * "Dissociate Token with Key #0?" (Summary) <--> Operator <-->
 * "Token" (Senders) <--> Fee <--> Memo <--> Confirm <--> Deny
 *
 * TokenMint:
 * "Mint Token with Key #0?" (Summary) <--> Operator <--> "Token" (Senders) <-->
 * Amount <--> Fee <--> Memo <--> Confirm <--> Deny
 *
 * TokenBurn:
 * "Burn Token with Key #0?" (Summary) <--> Operator <--> "Token" (Senders) <-->
 * Amount <--> Fee <--> Memo <--> Confirm <--> Deny
 *
 * I chose the steps for the originally supported CreateAccount and Transfer
transactions, and the additional transactions have been added since then. Steps
may be skipped or modified (as described above) from the original transfer flow.
 */

typedef struct sign_tx_context_s {
    // ui common
    uint32_t key_index;
    uint8_t transfer_to_index;
    uint8_t transfer_from_index;

    // Transaction Summary
    char summary_line_1[ DISPLAY_SIZE + 1 ];
    char summary_line_2[ DISPLAY_SIZE + 1 ];

#if defined(TARGET_NANOS)
    union {
#define TITLE_SIZE (DISPLAY_SIZE + 1)
        char title[ TITLE_SIZE ];
        char senders_title[ TITLE_SIZE ];    // alias for title
        char recipients_title[ TITLE_SIZE ]; // alias for title
        char amount_title[ TITLE_SIZE ];     // alias for title
    };
#elif defined(TARGET_NANOX) || defined(TARGET_NANOS2) || defined(TARGET_STAX)
    char senders_title[ DISPLAY_SIZE + 1 ];
    char recipients_title[ DISPLAY_SIZE + 1 ];
    char amount_title[ DISPLAY_SIZE + 1 ];
#endif

    // Account ID: uint64_t.uint64_t.uint64_t
    // Most other entities are shorter
#if defined(TARGET_NANOS)
    union {
#define FULL_SIZE (ACCOUNT_ID_SIZE + 1)
        char full[ FULL_SIZE ];
        char operator[ FULL_SIZE ];   // alias for full
        char senders[ FULL_SIZE ];    // alias for full
        char recipients[ FULL_SIZE ]; // alias for full
        char amount[ FULL_SIZE ];     // alias for full
        char fee[ FULL_SIZE ];        // alias for full
        char memo[ FULL_SIZE ];       // alias for full
    };
    char partial[ DISPLAY_SIZE + 1 ];
#endif

    // Steps correspond to parts of the transaction proto
    // type is set based on proto
#if defined(TARGET_NANOS)
    enum TransactionStep step;
#endif
    enum TransactionType type;

#if defined(TARGET_NANOS)
    uint8_t display_index; // 1 -> Number Screens
    uint8_t display_count; // Number Screens
#elif defined(TARGET_NANOX) || defined(TARGET_NANOS2) || defined(TARGET_STAX)
    // Transaction Operator
    char operator[ DISPLAY_SIZE * 2 + 1 ];

    // Transaction Senders
    char senders[ DISPLAY_SIZE * 2 + 1 ];

    // Transaction Recipients
    char recipients[ DISPLAY_SIZE * 2 + 1 ];

    // Transaction Amount
    char amount[ DISPLAY_SIZE * 2 + 1 ];

    // Transaction Fee
    char fee[ DISPLAY_SIZE * 2 + 1 ];

    // Transaction Memo
    char memo[ MAX_MEMO_SIZE + 1 ];
#endif

    // Parsed transaction
    Hedera_TransactionBody transaction;
} sign_tx_context_t;

extern sign_tx_context_t st_ctx;
