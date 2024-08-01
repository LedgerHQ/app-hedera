#include "sign_transaction.h"

sign_tx_context_t st_ctx;

// Validates whether or not a transfer is legal:
// Either a transfer between two accounts
// Or a token transfer between two accounts
static void validate_transfer(void) {
    if (st_ctx.transaction.data.cryptoTransfer.transfers.accountAmounts_count >
        2) {
        // More than two accounts in a transfer
        THROW(EXCEPTION_MALFORMED_APDU);
    }

    if (st_ctx.transaction.data.cryptoTransfer.transfers.accountAmounts_count ==
            2 &&
        st_ctx.transaction.data.cryptoTransfer.tokenTransfers_count != 0) {
        // Can't also transfer tokens while sending hbar
        THROW(EXCEPTION_MALFORMED_APDU);
    }

    if (st_ctx.transaction.data.cryptoTransfer.tokenTransfers_count > 1) {
        // More than one token transferred
        THROW(EXCEPTION_MALFORMED_APDU);
    }

    if (st_ctx.transaction.data.cryptoTransfer.tokenTransfers_count == 1) {
        if (st_ctx.transaction.data.cryptoTransfer.tokenTransfers[0]
                .transfers_count != 2) {
            // More than two accounts in a token transfer
            THROW(EXCEPTION_MALFORMED_APDU);
        }

        if (st_ctx.transaction.data.cryptoTransfer.transfers
                .accountAmounts_count != 0) {
            // Can't also transfer Hbar if the transaction is an otherwise valid
            // token transfer
            THROW(EXCEPTION_MALFORMED_APDU);
        }
    }
}

static bool is_verify_account(void) {
    // Only 1 Account (Sender), Fee 1 Tinybar, and Value 0 Tinybar
    return (
        st_ctx.transaction.data.cryptoTransfer.transfers.accountAmounts[0]
                .amount == 0 &&
        st_ctx.transaction.data.cryptoTransfer.transfers.accountAmounts_count ==
            1 &&
        st_ctx.transaction.transactionFee == 1);
}

static bool is_transfer(void) {
    // Number of Accounts == 2
    return (
        st_ctx.transaction.data.cryptoTransfer.transfers.accountAmounts_count ==
        2);
}

static bool is_token_transfer(void) {
    return (st_ctx.transaction.data.cryptoTransfer.tokenTransfers_count == 1);
}

void handle_transaction_body() {
    MEMCLEAR(st_ctx.summary_line_1);
    MEMCLEAR(st_ctx.summary_line_2);
#if defined(TARGET_NANOS)
    MEMCLEAR(st_ctx.full);
    MEMCLEAR(st_ctx.partial);
#else
    MEMCLEAR(st_ctx.amount_title);
    MEMCLEAR(st_ctx.senders_title);
    MEMCLEAR(st_ctx.operator);
    MEMCLEAR(st_ctx.senders);
    MEMCLEAR(st_ctx.recipients);
    MEMCLEAR(st_ctx.fee);
    MEMCLEAR(st_ctx.amount);
    MEMCLEAR(st_ctx.memo);
#endif

    // Step 1, Unknown Type, Screen 1 of 1
    st_ctx.type = Unknown;
#if defined(TARGET_NANOS)
    st_ctx.step = Summary;
    st_ctx.display_index = 1;
    st_ctx.display_count = 1;
#endif

    // <Do Action>
    // with Key #X?
    reformat_key();

#if !defined(TARGET_NANOS)
    // All flows except Verify
    if (!is_verify_account()) reformat_operator();
#endif

    // Handle parsed protobuf message of transaction body
    switch (st_ctx.transaction.which_data) {
        case Hedera_TransactionBody_cryptoCreateAccount_tag:
            st_ctx.type = Create;
            reformat_summary("Create Account");

#if !defined(TARGET_NANOS)
            reformat_stake_target();
            reformat_collect_rewards();
            reformat_amount_balance();
#endif
            break;

        case Hedera_TransactionBody_cryptoUpdateAccount_tag:
            st_ctx.type = Update;
            reformat_summary("Update Account");

#if !defined(TARGET_NANOS)
            reformat_stake_target();
            reformat_collect_rewards();
            reformat_updated_account();
#endif
            break;

        case Hedera_TransactionBody_tokenAssociate_tag:
            st_ctx.type = Associate;
            reformat_summary("Associate Token");

#if !defined(TARGET_NANOS)
            reformat_token_associate();
#endif
            break;

        case Hedera_TransactionBody_tokenDissociate_tag:
            st_ctx.type = Dissociate;
            reformat_summary("Dissociate Token");

#if !defined(TARGET_NANOS)
            reformat_token_dissociate();
#endif
            break;

        case Hedera_TransactionBody_tokenBurn_tag:
            st_ctx.type = TokenBurn;
            reformat_summary("Burn Token");

#if !defined(TARGET_NANOS)
            reformat_token_burn();
            reformat_amount_burn();
#endif
            break;

        case Hedera_TransactionBody_tokenMint_tag:
            st_ctx.type = TokenMint;
            reformat_summary("Mint Token");

#if !defined(TARGET_NANOS)
            reformat_token_mint();
            reformat_amount_mint();
#endif
            break;

        case Hedera_TransactionBody_cryptoTransfer_tag:
            validate_transfer(); // THROWs

            if (is_verify_account()) {
                st_ctx.type = Verify;
                reformat_summary("Verify Account");

#if !defined(TARGET_NANOS)
                reformat_verify_account();
#endif

            } else if (is_transfer()) {
                // Some other Transfer Transaction
                st_ctx.type = Transfer;
                reformat_summary("Send Hbar");

                // Determine Sender based on amount
                st_ctx.transfer_from_index = 0;
                st_ctx.transfer_to_index = 1;
                if (st_ctx.transaction.data.cryptoTransfer.transfers
                        .accountAmounts[0]
                        .amount > 0) {
                    st_ctx.transfer_from_index = 1;
                    st_ctx.transfer_to_index = 0;
                }

#if !defined(TARGET_NANOS)
                reformat_sender_account();
                reformat_recipient_account();
                reformat_amount_transfer();
#endif

            } else if (is_token_transfer()) {
                st_ctx.type = TokenTransfer;
                reformat_summary_send_token();

                // Determine Sender based on amount
                st_ctx.transfer_from_index = 0;
                st_ctx.transfer_to_index = 1;
                if (st_ctx.transaction.data.cryptoTransfer.tokenTransfers[0]
                        .transfers[0]
                        .amount > 0) {
                    st_ctx.transfer_from_index = 1;
                    st_ctx.transfer_to_index = 0;
                }

#if !defined(TARGET_NANOS)
                reformat_token_sender_account();
                reformat_token_recipient_account();
                reformat_token_transfer();
#endif

            } else {
                // Unsupported
                THROW(EXCEPTION_MALFORMED_APDU);
            }
            break;

        default:
            // Unsupported
            THROW(EXCEPTION_MALFORMED_APDU);
            break;
    }

#if !defined(TARGET_NANOS)
    // All flows except Verify
    if (!is_verify_account()) {
        reformat_fee();
        reformat_memo();
    }
#endif

    ui_sign_transaction();
}

// Sign Handler
// Decodes and handles transaction message
void handle_sign_transaction(uint8_t p1, uint8_t p2, uint8_t* buffer,
                             uint16_t len,
                             /* out */ volatile unsigned int* flags,
                             /* out */ volatile unsigned int* tx) {
    UNUSED(p1);
    UNUSED(p2);
    UNUSED(tx);

    // Raw Tx
    uint8_t raw_transaction[MAX_TX_SIZE];
    int raw_transaction_length = len - 4;

    // Oops Oof Owie
    if (raw_transaction_length > MAX_TX_SIZE ||
        raw_transaction_length > (int)buffer - 4 || buffer == NULL) {
        THROW(EXCEPTION_MALFORMED_APDU);
    }

    // Key Index
    st_ctx.key_index = U4LE(buffer, 0);

    // copy raw transaction
    memmove(raw_transaction, (buffer + 4), raw_transaction_length);

    // Sign Transaction
    if (!hedera_sign(st_ctx.key_index, raw_transaction, raw_transaction_length,
                     G_io_apdu_buffer)) {
        THROW(EXCEPTION_MALFORMED_APDU);
    }

    // Make in memory buffer into stream
    pb_istream_t stream =
        pb_istream_from_buffer(raw_transaction, raw_transaction_length);

    // Decode the Transaction
    if (!pb_decode(&stream, Hedera_TransactionBody_fields,
                   &st_ctx.transaction)) {
        // Oh no couldn't ...
        THROW(EXCEPTION_MALFORMED_APDU);
    }

    handle_transaction_body();

    *flags |= IO_ASYNCH_REPLY;
}
