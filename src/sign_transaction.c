#include "sign_transaction.h"

#include <swap_entrypoints.h>
#include <swap_utils.h>

#include "handle_swap_sign_transaction.h"
#include "proto_varlen_parser.h"
#include "tokens/cal/token_lookup.h"

sign_tx_context_t st_ctx;

static void parse_and_lookup_token(token_addr_t* token_addr) {
    if (token_addr == NULL) {
        return;
    }

    // Parse token address to string
    address_to_string(token_addr, st_ctx.token_address_str);

    // Get info about token
    st_ctx.token_known =
        token_info_get_by_address(*token_addr, st_ctx.token_ticker,
                                  st_ctx.token_name, &st_ctx.token_decimals);
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

static void validate_crypto_update(void) {
    // Validate account is not 0.0.0
    if (st_ctx.transaction.data.cryptoUpdateAccount.accountIDToUpdate
            .which_account == 3) {
        int64_t account_num = st_ctx.transaction.data.cryptoUpdateAccount
                                  .accountIDToUpdate.account.accountNum;
        int64_t realm_num = st_ctx.transaction.data.cryptoUpdateAccount
                                .accountIDToUpdate.realmNum;
        int64_t shard_num = st_ctx.transaction.data.cryptoUpdateAccount
                                .accountIDToUpdate.shardNum;

        if (account_num == 0 && realm_num == 0 && shard_num == 0) {
            THROW(EXCEPTION_MALFORMED_APDU);
        }
    }

    // Currently we don't support updating the key, because it requires double
    // signing
    if (st_ctx.transaction.data.cryptoUpdateAccount.has_key) {
        THROW(EXCEPTION_MALFORMED_APDU);
    }
}

void handle_transaction_body() {
    MEMCLEAR(st_ctx.summary_line_1);
    MEMCLEAR(st_ctx.summary_line_2);
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
    MEMCLEAR(st_ctx.auto_renew_period);
    MEMCLEAR(st_ctx.expiration_time);
    MEMCLEAR(st_ctx.receiver_sig_required);
    MEMCLEAR(st_ctx.max_auto_token_assoc);

    // Step 1, Unknown Type, Screen 1 of 1
    st_ctx.type = Unknown;

    // Legacy flow with Summary
    // 1.<Do Action>
    //   with Key #X?
    reformat_key();

    // New flow with Key Index
    // 1. Review transaction
    // 2. With Key #X?
    reformat_key_index();

    // All flows except Verify
    if (!is_verify_account()) reformat_operator();

    // Handle parsed protobuf message of transaction body
    switch (st_ctx.transaction.which_data) {
        case Hedera_TransactionBody_cryptoCreateAccount_tag:
            st_ctx.type = Create;
            reformat_summary("Create Account");

            reformat_stake_target();
            reformat_collect_rewards();
            reformat_amount_balance();
            break;

        case Hedera_TransactionBody_cryptoUpdateAccount_tag:
            validate_crypto_update(); // THROWs

            st_ctx.type = Update;
            st_ctx.update_type = identify_special_update(
                &st_ctx.transaction.data.cryptoUpdateAccount);
            switch (st_ctx.update_type) {
                case STAKE_UPDATE:
                    reformat_summary("stake Hbar");
                    reformat_account_to_update();
                    reformat_stake_in_stake_flow();
                    reformat_collect_rewards_in_stake_flow();
                    break;
                case UNSTAKE_UPDATE:
                    reformat_summary("unstake Hbar");
                    reformat_unstake_account_to_update();
                    reformat_collect_rewards_in_stake_flow();
                    break;
                default:
                    reformat_summary("update account");
                    reformat_updated_account();
                    reformat_stake_target();
                    reformat_auto_renew_period();
                    reformat_expiration_time();
                    reformat_receiver_sig_required();
                    reformat_max_automatic_token_associations();
                    reformat_collect_rewards();
                    break;
            }
            break;

        case Hedera_TransactionBody_tokenAssociate_tag:
            st_ctx.type = Associate;
            reformat_summary("associate token");

            token_addr_t associate_token_address = {
                st_ctx.transaction.data.tokenAssociate.tokens[0].shardNum,
                st_ctx.transaction.data.tokenAssociate.tokens[0].realmNum,
                st_ctx.transaction.data.tokenAssociate.tokens[0].tokenNum,
            };
            parse_and_lookup_token(&associate_token_address);
            reformat_token_associate();
            break;

        case Hedera_TransactionBody_tokenDissociate_tag:
            st_ctx.type = Dissociate;
            reformat_summary("Dissociate Token");

            token_addr_t dissociate_token_address = {
                st_ctx.transaction.data.tokenDissociate.tokens[0].shardNum,
                st_ctx.transaction.data.tokenDissociate.tokens[0].realmNum,
                st_ctx.transaction.data.tokenDissociate.tokens[0].tokenNum,
            };
            parse_and_lookup_token(&dissociate_token_address);
            reformat_token_dissociate();
            break;

        case Hedera_TransactionBody_tokenBurn_tag:
            st_ctx.type = TokenBurn;
            reformat_summary("Burn Token");

            reformat_token_burn();
            reformat_amount_burn();
            break;

        case Hedera_TransactionBody_tokenMint_tag:
            st_ctx.type = TokenMint;
            reformat_summary("Mint Token");

            reformat_token_mint();
            reformat_amount_mint();
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

                reformat_sender_account();
                reformat_recipient_account();
                reformat_amount_transfer();

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

                reformat_token_sender_account();
                reformat_token_recipient_account();
                reformat_token_transfer();

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

    // All flows except Verify
    if (!is_verify_account()) {
        reformat_fee();
        reformat_memo();
    }

#ifdef HAVE_SWAP
    // If we are in swap context, do not redisplay the message data
    // Instead, ensure they are identical with what was previously displayed
    if (G_called_from_swap) {
        if (G_swap_response_ready) {
            // Safety against trying to make the app sign multiple TX
            // This code should never be triggered as the app is supposed to
            // exit after sending the signed transaction
            PRINTF("Safety against double signing triggered\n");
            os_sched_exit(-1);
        } else {
            // We will quit the app after this transaction, whether it succeeds
            // or fails
            PRINTF(
                "Swap response is ready, the app will quit after the next "
                "send\n");
            // This boolean will make the io_send_sw family instant reply +
            // return to exchange
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

    // Checking input parameters
    if ((buffer == NULL) || (len <= INDEX_SIZE)) {
        PRINTF("%s: wrong buffer pointer or input length\n", __func__);
        THROW(EXCEPTION_MALFORMED_APDU);
    }

    uint16_t raw_transaction_length = len - INDEX_SIZE;
    // Checking transaction length
    // Unnecessary check today as MAX_TX_SIZE is set to 512 bytes,
    // but APDU length field occupies only 1 byte.
    // TODO: to set correct MAX_TX_SIZE and maybe to transform uint16_t len to
    // uint8_t
    if (raw_transaction_length > MAX_TX_SIZE) {
        PRINTF("%s: too big transaction length\n", __func__);
        THROW(EXCEPTION_MALFORMED_APDU);
    }

    // Key Index (Little Endian format)
    st_ctx.key_index = U4LE(buffer, 0);

    // copy raw transaction.
    // using simply memcpy() as the overlap between the
    // local and global buffers is impossible.
    memcpy(raw_transaction, (buffer + INDEX_SIZE), raw_transaction_length);

    // Sign Transaction
    if (!hedera_sign(st_ctx.key_index, raw_transaction, raw_transaction_length,
                     G_io_apdu_buffer, &st_ctx.signature_length)) {
        PRINTF("%s: signature failure\n", __func__);
        THROW(EXCEPTION_MALFORMED_APDU);
    }

    // Make in memory buffer into stream
    pb_istream_t stream =
        pb_istream_from_buffer(raw_transaction, raw_transaction_length);

    // Decode the Transaction
    if (!pb_decode(&stream, Hedera_TransactionBody_fields,
                   &st_ctx.transaction)) {
        // Oh no couldn't ...
        PRINTF("%s: decoding failure\n", __func__);
        THROW(EXCEPTION_MALFORMED_APDU);
    }

    // Extract account memo from cryptoUpdateAccount using second-stage protobuf
    // decoding This handles the nested StringValue structure that nanopb
    // doesn't decode automatically
    if (st_ctx.transaction.which_data ==
        Hedera_TransactionBody_cryptoUpdateAccount_tag) {
        if (!extract_nested_string_field(
                raw_transaction, raw_transaction_length, 14,
                st_ctx.account_memo, sizeof(st_ctx.account_memo))) {
            strcpy(st_ctx.account_memo, "-");
        }
    }

    handle_transaction_body();

    *flags |= IO_ASYNCH_REPLY;
}
