#include "handle_swap_sign_transaction.h"
#include "sign_transaction.h"
#include "tokens/cal/token_lookup.h"
#include <swap_entrypoints.h>
#include <swap_utils.h>


#include <swap_entrypoints.h>
#include <swap_utils.h>


sign_tx_context_t st_ctx;

#if !defined(TARGET_NANOS)
static void parse_and_lookup_token(token_addr_t* token_addr) {
    // Parse token address to string
    address_to_string(token_addr, st_ctx.token_address_str);

    // Get info about token
    st_ctx.token_known = token_info_get_by_address(
        *token_addr, st_ctx.token_ticker, st_ctx.token_name,
        &st_ctx.token_decimals);
}
#endif

static void write_u16_be(uint8_t *ptr, size_t offset, uint16_t value) {
    ptr[offset + 0] = (uint8_t) (value >> 8);
    ptr[offset + 1] = (uint8_t) (value >> 0);
}

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
    MEMCLEAR(st_ctx.token_decimals);
    MEMCLEAR(st_ctx.token_name);
    MEMCLEAR(st_ctx.token_ticker);
    MEMCLEAR(st_ctx.token_address_str);
    MEMCLEAR(st_ctx.token_known);
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
            reformat_summary("associate token");

#if !defined(TARGET_NANOS)
            token_addr_t associate_token_address = {
                st_ctx.transaction.data.tokenAssociate.tokens[0].shardNum,
                st_ctx.transaction.data.tokenAssociate.tokens[0].realmNum,
                st_ctx.transaction.data.tokenAssociate.tokens[0].tokenNum,
            };
            parse_and_lookup_token(&associate_token_address);
            reformat_token_associate();
#endif
            break;

        case Hedera_TransactionBody_tokenDissociate_tag:
            st_ctx.type = Dissociate;
            reformat_summary("Dissociate Token");

#if !defined(TARGET_NANOS)
            token_addr_t dissociate_token_address = {
                st_ctx.transaction.data.tokenDissociate.tokens[0].shardNum,
                st_ctx.transaction.data.tokenDissociate.tokens[0].realmNum,
                st_ctx.transaction.data.tokenDissociate.tokens[0].tokenNum,
            };
            parse_and_lookup_token(&dissociate_token_address);
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

                token_addr_t token_address = {
                    st_ctx.transaction.data.cryptoTransfer.tokenTransfers[0]
                        .token.shardNum,
                    st_ctx.transaction.data.cryptoTransfer.tokenTransfers[0]
                        .token.realmNum,
                    st_ctx.transaction.data.cryptoTransfer.tokenTransfers[0]
                        .token.tokenNum,
                };
                parse_and_lookup_token(&token_address);
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

#ifdef HAVE_SWAP
    // If we are in swap context, do not redisplay the message data
    // Instead, ensure they are identical with what was previously displayed
    if (G_called_from_swap) {
        if (G_swap_response_ready) {
            // Safety against trying to make the app sign multiple TX
            // This code should never be triggered as the app is supposed to exit after
            // sending the signed transaction
            PRINTF("Safety against double signing triggered\n");
            os_sched_exit(-1);
        } else {
            // We will quit the app after this transaction, whether it succeeds or fails
            PRINTF("Swap response is ready, the app will quit after the next send\n");
            // This boolean will make the io_send_sw family instant reply + return to
            // exchange
            G_swap_response_ready = true;
        }
        if (swap_check_validity()) {
            PRINTF("Swap response validated\n");
            validate_transfer();

            uint8_t tx = st_ctx.signature_length;

            U2BE_ENCODE(G_io_apdu_buffer, tx, EXCEPTION_OK);
            tx += 2;

            // Send back the response, do not restart the event loop
            io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, tx);
            finalize_exchange_sign_transaction(true);
        } else {
            PRINTF("swap_check_validity failed\n");
            uint8_t tx = 0;

            U2BE_ENCODE(G_io_apdu_buffer, tx, EXCEPTION_INTERNAL);
            tx += 2;

            // Send back the response, do not restart the event loop
            io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, tx);

            finalize_exchange_sign_transaction(false);
        }
    } else {
        ui_sign_transaction();
    }
#else
    ui_sign_transaction();
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
                     G_io_apdu_buffer, &st_ctx.signature_length)) {
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
