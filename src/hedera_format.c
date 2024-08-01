#include "hedera_format.h"

#define BUF_SIZE 32

static char *hedera_format_amount(uint64_t amount, uint8_t decimals) {
    static char buf[BUF_SIZE];

    // NOTE: format of amounts are not sensitive
    memset(buf, 0, BUF_SIZE);

    // Quick shortcut if the amount is zero
    // Regardless of decimals, the output is always "0"
    if (amount == 0) {
        buf[0] = '0';
        buf[1] = '\0';

        return buf;
    }

    // NOTE: we silently fail with a decimal value > 20
    //  this function shuold only be called on decimal values smaller than 20
    if (decimals >= 20) return buf;

    int i = 0;

    while (i < (BUF_SIZE - 1) && (amount > 0 || i < decimals)) {
        int digit = amount % 10;
        amount /= 10;

        buf[i++] = '0' + digit;

        if (i == decimals) {
            buf[i++] = '.';
        }
    }

    if (buf[i - 1] == '.') {
        buf[i++] = '0';
    }

    int size = i;
    int j = 0;
    char tmp;

    while (j < i) {
        i -= 1;

        tmp = buf[j];
        buf[j] = buf[i];
        buf[i] = tmp;

        j += 1;
    }

    for (j = size - 1; j > 0; j--) {
        if (buf[j] == '0') {
            continue;
        } else if (buf[j] == '.') {
            break;
        } else {
            j += 1;
            break;
        }
    }

    if (j < size - 1) {
        buf[j] = '\0';
    }

    return buf;
}

static char *hedera_format_tinybar(uint64_t tinybar) {
    return hedera_format_amount(tinybar, 8);
}

static void validate_decimals(uint32_t decimals) {
    if (decimals >= 20) {
        // We only support decimal values less than 20
        THROW(EXCEPTION_MALFORMED_APDU);
    }
}

static void validate_memo(const char memo[100]) {
    if (strlen(memo) > MAX_MEMO_SIZE) {
        // Hedera max length for memos
        THROW(EXCEPTION_MALFORMED_APDU);
    }
}

#define hedera_safe_printf(element, ...) \
    hedera_snprintf(element, sizeof(element) - 1, __VA_ARGS__)

void reformat_key(void) {
    hedera_safe_printf(st_ctx.summary_line_2,
#if defined(TARGET_NANOX) || defined(TARGET_NANOS2) || defined(TARGET_NANOS)
                       "with Key #%u?",
#elif defined(TARGET_STAX) || defined(TARGET_FLEX)
                       "#%u",
#endif
                       st_ctx.key_index);
}

// SUMMARIES

void reformat_summary(const char *summary) {
    hedera_safe_printf(st_ctx.summary_line_1, summary);
}

void reformat_summary_send_token(void) {
    hedera_safe_printf(
#if defined(TARGET_NANOX) || defined(TARGET_NANOS2) || defined(TARGET_NANOS)
        st_ctx.summary_line_1, "Send %llu.%llu.%llu",
#elif defined(TARGET_STAX) || defined(TARGET_FLEX)
        st_ctx.summary_line_1, "Send Token %llu.%llu.%llu",
#endif
        st_ctx.transaction.data.cryptoTransfer.tokenTransfers[0].token.shardNum,
        st_ctx.transaction.data.cryptoTransfer.tokenTransfers[0].token.realmNum,
        st_ctx.transaction.data.cryptoTransfer.tokenTransfers[0]
            .token.tokenNum);
}

// TITLES

#if defined(TARGET_NANOS)
static void set_title(const char *title) {
    hedera_safe_printf(st_ctx.title, "%s (%u/%u)", title, st_ctx.display_index,
                       st_ctx.display_count);
}
#endif

static void set_senders_title(const char *title) {
#if defined(TARGET_NANOS)
    set_title(title);
#else
    // st_ctx.senders_title --> st_ctx.title (NANOS)
    hedera_safe_printf(st_ctx.senders_title, "%s", title);
#endif
}

static void set_recipients_title(const char *title) {
#if defined(TARGET_NANOS)
    set_title(title);
#else
    // st_ctx.recipients_title --> st_ctx.title (NANOS)
    hedera_safe_printf(st_ctx.recipients_title, "%s", title);
#endif
}

static void set_amount_title(const char *title) {
#if defined(TARGET_NANOS)
    set_title(title);
#else
    // st_ctx.senders_title --> st_ctx.title (NANOS)
    hedera_safe_printf(st_ctx.amount_title, "%s", title);
#endif
}

// OPERATOR

void reformat_operator(void) {
#if defined(TARGET_NANOS)
    set_title("Operator");
#endif

    // st_ctx.operator --> st_ctx.full (NANOS)
    hedera_safe_printf(st_ctx.operator, "%llu.%llu.%llu",
                       st_ctx.transaction.transactionID.accountID.shardNum,
                       st_ctx.transaction.transactionID.accountID.realmNum,
                       st_ctx.transaction.transactionID.accountID.account);
}

// SENDERS

void reformat_stake_target(void) {
    set_senders_title("Stake To");

    if (st_ctx.type == Create) {
        // st_ctx.senders --> st_ctx.full (NANOS)
        if (st_ctx.transaction.data.cryptoCreateAccount.which_staked_id ==
            Hedera_CryptoCreateTransactionBody_staked_account_id_tag) {
            // An account ID and not a Node ID
            hedera_safe_printf(
                st_ctx.senders, "%llu.%llu.%llu",
                st_ctx.transaction.data.cryptoCreateAccount.staked_id
                    .staked_account_id.shardNum,
                st_ctx.transaction.data.cryptoCreateAccount.staked_id
                    .staked_account_id.realmNum,
                st_ctx.transaction.data.cryptoCreateAccount.staked_id
                    .staked_account_id.account.accountNum);
        } else if (st_ctx.transaction.data.cryptoCreateAccount
                       .which_staked_id ==
                   Hedera_CryptoCreateTransactionBody_staked_node_id_tag) {
            hedera_safe_printf(st_ctx.senders, "Node %lld",
                               st_ctx.transaction.data.cryptoCreateAccount
                                   .staked_id.staked_node_id);
        }
    } else if (st_ctx.type == Update) {
        if (st_ctx.transaction.data.cryptoUpdateAccount.which_staked_id ==
            Hedera_CryptoUpdateTransactionBody_staked_account_id_tag) {
            hedera_safe_printf(
                st_ctx.senders, "%llu.%llu.%llu",
                st_ctx.transaction.data.cryptoUpdateAccount.staked_id
                    .staked_account_id.shardNum,
                st_ctx.transaction.data.cryptoUpdateAccount.staked_id
                    .staked_account_id.realmNum,
                st_ctx.transaction.data.cryptoUpdateAccount.staked_id
                    .staked_account_id.account.accountNum);
        } else if (st_ctx.transaction.data.cryptoUpdateAccount
                       .which_staked_id ==
                   Hedera_CryptoUpdateTransactionBody_staked_node_id_tag) {
            hedera_safe_printf(st_ctx.senders, "Node %lld",
                               st_ctx.transaction.data.cryptoUpdateAccount
                                   .staked_id.staked_node_id);
        }
    }
}

void reformat_token_associate(void) {
    set_senders_title("Token");

    // st_ctx.senders --> st_ctx.full (NANOS)
    hedera_safe_printf(
        st_ctx.senders, "%llu.%llu.%llu",
        st_ctx.transaction.data.tokenAssociate.tokens[0].shardNum,
        st_ctx.transaction.data.tokenAssociate.tokens[0].realmNum,
        st_ctx.transaction.data.tokenAssociate.tokens[0].tokenNum);
}

void reformat_token_dissociate(void) {
    set_senders_title("Token");

    // st_ctx.senders --> st_ctx.full (NANOS)
    hedera_safe_printf(
        st_ctx.senders, "%llu.%llu.%llu",
        st_ctx.transaction.data.tokenDissociate.tokens[0].shardNum,
        st_ctx.transaction.data.tokenDissociate.tokens[0].realmNum,
        st_ctx.transaction.data.tokenDissociate.tokens[0].tokenNum);
}

void reformat_token_mint(void) {
    set_senders_title("Token");

    // st_ctx.senders --> st_ctx.full (NANOS)
    hedera_safe_printf(st_ctx.senders, "%llu.%llu.%llu",
                       st_ctx.transaction.data.tokenMint.token.shardNum,
                       st_ctx.transaction.data.tokenMint.token.realmNum,
                       st_ctx.transaction.data.tokenMint.token.tokenNum);
}

void reformat_token_burn(void) {
    set_senders_title("Token");

    // st_ctx.senders --> st_ctx.full (NANOS)
    hedera_safe_printf(st_ctx.senders, "%llu.%llu.%llu",
                       st_ctx.transaction.data.tokenBurn.token.shardNum,
                       st_ctx.transaction.data.tokenBurn.token.realmNum,
                       st_ctx.transaction.data.tokenBurn.token.tokenNum);
}

void reformat_verify_account() {
    set_senders_title("Account");

    // st_ctx.senders --> st_ctx.full (NANOS)
    hedera_safe_printf(
        st_ctx.senders, "%llu.%llu.%llu",
        st_ctx.transaction.data.cryptoTransfer.transfers.accountAmounts[0]
            .accountID.shardNum,
        st_ctx.transaction.data.cryptoTransfer.transfers.accountAmounts[0]
            .accountID.realmNum,
        st_ctx.transaction.data.cryptoTransfer.transfers.accountAmounts[0]
            .accountID.account);
}

void reformat_sender_account(void) {
    set_senders_title("Account");

    // st_ctx.senders --> st_ctx.full (NANOS)
    hedera_safe_printf(st_ctx.senders, "%llu.%llu.%llu",
                       st_ctx.transaction.data.cryptoTransfer.transfers
                           .accountAmounts[st_ctx.transfer_from_index]
                           .accountID.shardNum,
                       st_ctx.transaction.data.cryptoTransfer.transfers
                           .accountAmounts[st_ctx.transfer_from_index]
                           .accountID.realmNum,
                       st_ctx.transaction.data.cryptoTransfer.transfers
                           .accountAmounts[st_ctx.transfer_from_index]
                           .accountID.account);
}

void reformat_token_sender_account(void) {
    set_senders_title("Sender");

    // st_ctx.senders --> st_ctx.full (NANOS)
    hedera_safe_printf(st_ctx.senders, "%llu.%llu.%llu",
                       st_ctx.transaction.data.cryptoTransfer.tokenTransfers[0]
                           .transfers[st_ctx.transfer_from_index]
                           .accountID.shardNum,
                       st_ctx.transaction.data.cryptoTransfer.tokenTransfers[0]
                           .transfers[st_ctx.transfer_from_index]
                           .accountID.realmNum,
                       st_ctx.transaction.data.cryptoTransfer.tokenTransfers[0]
                           .transfers[st_ctx.transfer_from_index]
                           .accountID.account);
}

// RECIPIENTS

void reformat_collect_rewards(void) {
    set_recipients_title("Collect Rewards?");

    if (st_ctx.type == Create) {
        // st_ctx.recipients --> st_ctx.full (NANOS)
        bool declineRewards =
            st_ctx.transaction.data.cryptoCreateAccount.decline_reward;
        // Collect Rewards? ('not decline rewards'?) Yes / No
        hedera_safe_printf(st_ctx.recipients, "%s",
                           !declineRewards ? "Yes" : "No");
    } else if (st_ctx.type == Update) {
        if (st_ctx.transaction.data.cryptoUpdateAccount.has_decline_reward) {
            bool declineRewards = st_ctx.transaction.data.cryptoUpdateAccount
                                      .decline_reward.value;
            // Collect Rewards? ('not decline rewards'?) Yes / No
            hedera_safe_printf(st_ctx.recipients, "%s",
                               !declineRewards ? "Yes" : "No");
        } else {
            hedera_safe_printf(st_ctx.recipients, "%s", "-");
        }
    }
}

void reformat_recipient_account(void) {
    set_recipients_title("Recipient");

    // st_ctx.recipients --> st_ctx.full (NANOS)
    hedera_safe_printf(st_ctx.recipients, "%llu.%llu.%llu",
                       st_ctx.transaction.data.cryptoTransfer.transfers
                           .accountAmounts[st_ctx.transfer_to_index]
                           .accountID.shardNum,
                       st_ctx.transaction.data.cryptoTransfer.transfers
                           .accountAmounts[st_ctx.transfer_to_index]
                           .accountID.realmNum,
                       st_ctx.transaction.data.cryptoTransfer.transfers
                           .accountAmounts[st_ctx.transfer_to_index]
                           .accountID.account);
}

void reformat_token_recipient_account(void) {
    set_recipients_title("Recipient");

    // st_ctx.recipients --> st_ctx.full (NANOS)
    hedera_safe_printf(st_ctx.recipients, "%llu.%llu.%llu",
                       st_ctx.transaction.data.cryptoTransfer.tokenTransfers[0]
                           .transfers[st_ctx.transfer_to_index]
                           .accountID.shardNum,
                       st_ctx.transaction.data.cryptoTransfer.tokenTransfers[0]
                           .transfers[st_ctx.transfer_to_index]
                           .accountID.realmNum,
                       st_ctx.transaction.data.cryptoTransfer.tokenTransfers[0]
                           .transfers[st_ctx.transfer_to_index]
                           .accountID.account);
}

// AMOUNTS

void reformat_updated_account(void) {
    set_amount_title("Updating");

    if (st_ctx.transaction.data.cryptoUpdateAccount.has_accountIDToUpdate) {
        hedera_safe_printf(st_ctx.amount, "%llu.%llu.%llu",
                           st_ctx.transaction.data.cryptoUpdateAccount
                               .accountIDToUpdate.shardNum,
                           st_ctx.transaction.data.cryptoUpdateAccount
                               .accountIDToUpdate.realmNum,
                           st_ctx.transaction.data.cryptoUpdateAccount
                               .accountIDToUpdate.account.accountNum);
    } else {
        // No target, default Operator
        hedera_safe_printf(
            st_ctx.amount, "%llu.%llu.%llu",
            st_ctx.transaction.transactionID.accountID.shardNum,
            st_ctx.transaction.transactionID.accountID.realmNum,
            st_ctx.transaction.transactionID.accountID.account.accountNum);
    }
}

void reformat_amount_balance(void) {
    set_amount_title("Balance");

    // st_ctx.amount --> st_ctx.full (NANOS)
    hedera_safe_printf(
        st_ctx.amount, "%s hbar",
        hedera_format_tinybar(
            st_ctx.transaction.data.cryptoCreateAccount.initialBalance));
}

void reformat_amount_transfer(void) {
    set_amount_title("Amount");

    // st_ctx.amount --> st_ctx.full (NANOS)
    hedera_safe_printf(
        st_ctx.amount, "%s hbar",
        hedera_format_tinybar(st_ctx.transaction.data.cryptoTransfer.transfers
                                  .accountAmounts[st_ctx.transfer_to_index]
                                  .amount));
}

void reformat_amount_burn(void) {
    set_amount_title("Amount");

    // st_ctx.amount --> st_ctx.full (NANOS)
    hedera_safe_printf(
        st_ctx.amount, "%s",
        hedera_format_amount(st_ctx.transaction.data.tokenBurn.amount,
                             0)); // Always lowest denomination
}

void reformat_amount_mint(void) {
    set_amount_title("Amount");

    // st_ctx.amount --> st_ctx.full (NANOS)
    hedera_safe_printf(
        st_ctx.amount, "%s",
        hedera_format_amount(st_ctx.transaction.data.tokenMint.amount,
                             0)); // Always lowest denomination
}

void reformat_token_transfer(void) {
    validate_decimals(st_ctx.transaction.data.cryptoTransfer.tokenTransfers[0]
                          .expected_decimals.value);
    set_amount_title("Amount");

    // st_ctx.amount --> st_ctx.full (NANOS)
    hedera_safe_printf(
        st_ctx.amount, "%s",
        hedera_format_amount(
            st_ctx.transaction.data.cryptoTransfer.tokenTransfers[0]
                .transfers[st_ctx.transfer_to_index]
                .amount,
            st_ctx.transaction.data.cryptoTransfer.tokenTransfers[0]
                .expected_decimals.value));
}

// FEE

void reformat_fee(void) {
#if defined(TARGET_NANOS)
    set_title("Max Fee");
#endif
    // st_ctx.fee --> st_ctx.full (NANOS)
    hedera_safe_printf(
        st_ctx.fee, "%s hbar",
        hedera_format_tinybar(st_ctx.transaction.transactionFee));
}

// MEMO

void reformat_memo(void) {
    validate_memo(st_ctx.transaction.memo);

#if defined(TARGET_NANOS)
    set_title("Memo");
#endif

    // st_ctx.memo --> st_ctx.full (NANOS)
    hedera_safe_printf(
        st_ctx.memo, "%s",
        (st_ctx.transaction.memo[0] != '\0') ? st_ctx.transaction.memo : "");
}
