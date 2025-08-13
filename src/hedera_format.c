#include "hedera_format.h"

#include "staking.h"
#include "time_format.h"

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
#if defined(TARGET_NANOX) || defined(TARGET_NANOS2) || defined(TARGET_NANOS)
    hedera_safe_printf(st_ctx.summary_line_2, "with Key #%u?", st_ctx.key_index);
#elif defined(TARGET_STAX) || defined(TARGET_FLEX)
    hedera_safe_printf(st_ctx.summary_line_2, "#%u", st_ctx.key_index);
#endif

    hedera_safe_printf(st_ctx.key_index_str, "#%u", st_ctx.key_index);
}

void reformat_key_index(void) {
    hedera_safe_printf(st_ctx.key_index_str, "#%u", st_ctx.key_index);
}

// SUMMARIES

void reformat_summary(const char *summary) {
    hedera_safe_printf(st_ctx.summary_line_1, summary);
}

void reformat_summary_send_token(void) {
    hedera_safe_printf(st_ctx.summary_line_1, "send tokens");   
}

// TITLES



static void set_senders_title(const char *title) {
    // st_ctx.senders_title --> st_ctx.title (NANOS)
    hedera_safe_printf(st_ctx.senders_title, "%s", title);
}

static void set_recipients_title(const char *title) {
    // st_ctx.recipients_title --> st_ctx.title (NANOS)
    hedera_safe_printf(st_ctx.recipients_title, "%s", title);
}

static void set_amount_title(const char *title) {
    // st_ctx.senders_title --> st_ctx.title (NANOS)
    hedera_safe_printf(st_ctx.amount_title, "%s", title);
}

// OPERATOR

void reformat_operator(void) {
    // st_ctx.operator --> st_ctx.full (NANOS)
    hedera_safe_printf(st_ctx.operator, "%llu.%llu.%llu",
                       st_ctx.transaction.transactionID.accountID.shardNum,
                       st_ctx.transaction.transactionID.accountID.realmNum,
                       st_ctx.transaction.transactionID.accountID.account);
}

// SENDERS

void reformat_stake_target(void) {
    set_senders_title("Stake to");

    if (st_ctx.type == Create) {
        // st_ctx.senders --> st_ctx.full (NANOS)
        if (st_ctx.transaction.data.cryptoCreateAccount.which_staked_id ==
            Hedera_CryptoCreateTransactionBody_staked_account_id_tag) {
            // An account ID and not a Node ID
            // Check if this is the special Ledger account
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
        } else {
            hedera_safe_printf(st_ctx.senders, "-");
        }
    } else if (st_ctx.type == Update) {
        if (st_ctx.transaction.data.cryptoUpdateAccount.which_staked_id ==
            Hedera_CryptoUpdateTransactionBody_staked_account_id_tag) {
            // Check if this is the special Ledger account
            if (is_ledger_account(&st_ctx.transaction.data.cryptoUpdateAccount
                                       .staked_id.staked_account_id)) {
                hedera_safe_printf(
                    st_ctx.senders, "Ledger by %llu.%llu.%llu",
                    st_ctx.transaction.data.cryptoUpdateAccount.staked_id
                        .staked_account_id.shardNum,
                    st_ctx.transaction.data.cryptoUpdateAccount.staked_id
                        .staked_account_id.realmNum,
                    st_ctx.transaction.data.cryptoUpdateAccount.staked_id
                        .staked_account_id.account.accountNum);
            } else {
                hedera_safe_printf(
                    st_ctx.senders, "%llu.%llu.%llu",
                    st_ctx.transaction.data.cryptoUpdateAccount.staked_id
                        .staked_account_id.shardNum,
                    st_ctx.transaction.data.cryptoUpdateAccount.staked_id
                        .staked_account_id.realmNum,
                    st_ctx.transaction.data.cryptoUpdateAccount.staked_id
                        .staked_account_id.account.accountNum);
            }
        } else if (st_ctx.transaction.data.cryptoUpdateAccount
                       .which_staked_id ==
                   Hedera_CryptoUpdateTransactionBody_staked_node_id_tag) {
            hedera_safe_printf(st_ctx.senders, "Node %lld",
                               st_ctx.transaction.data.cryptoUpdateAccount
                                   .staked_id.staked_node_id);
        } else {
            hedera_safe_printf(st_ctx.senders, "-");
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
    set_senders_title("From");

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

void address_to_string(const token_addr_t *addr,
                       char buf[MAX_HEDERA_ADDRESS_LENGTH + 1]) {
    if (addr == NULL || buf == NULL) {
        return;
    }
    
    hedera_snprintf(buf, MAX_HEDERA_ADDRESS_LENGTH, "%llu.%llu.%llu",
                    addr->addr_shard, addr->addr_realm, addr->addr_account);
}

void reformat_token_sender_account(void) {
    set_senders_title("From");

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

void reformat_stake_in_stake_flow(void) {
    set_recipients_title("Stake to");
    if (st_ctx.transaction.data.cryptoUpdateAccount.which_staked_id ==
        Hedera_CryptoUpdateTransactionBody_staked_account_id_tag) {
        // Check if this is the special Ledger account
        if (is_ledger_account(&st_ctx.transaction.data.cryptoUpdateAccount
                                   .staked_id.staked_account_id)) {
            hedera_safe_printf(
                st_ctx.recipients, "Ledger by %llu.%llu.%llu",
                st_ctx.transaction.data.cryptoUpdateAccount.staked_id
                    .staked_account_id.shardNum,
                st_ctx.transaction.data.cryptoUpdateAccount.staked_id
                    .staked_account_id.realmNum,
                st_ctx.transaction.data.cryptoUpdateAccount.staked_id
                    .staked_account_id.account.accountNum);
        } else {
            hedera_safe_printf(
                st_ctx.recipients, "%llu.%llu.%llu",
                st_ctx.transaction.data.cryptoUpdateAccount.staked_id
                    .staked_account_id.shardNum,
                st_ctx.transaction.data.cryptoUpdateAccount.staked_id
                    .staked_account_id.realmNum,
                st_ctx.transaction.data.cryptoUpdateAccount.staked_id
                    .staked_account_id.account.accountNum);
        }
    } else if (st_ctx.transaction.data.cryptoUpdateAccount.which_staked_id ==
               Hedera_CryptoUpdateTransactionBody_staked_node_id_tag) {
        // TODO Node name
        hedera_safe_printf(st_ctx.recipients, "Node %lld",
                           st_ctx.transaction.data.cryptoUpdateAccount.staked_id
                               .staked_node_id);
    }
}

void reformat_collect_rewards(void) {
    set_recipients_title("Collect rewards?");

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
    set_recipients_title("To");

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
    set_recipients_title("To");

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

void reformat_account_to_update(void) {
    set_amount_title("Account");

    hedera_safe_printf(
        st_ctx.amount, "%llu.%llu.%llu",
        st_ctx.transaction.data.cryptoUpdateAccount.accountIDToUpdate.shardNum,
        st_ctx.transaction.data.cryptoUpdateAccount.accountIDToUpdate.realmNum,
        st_ctx.transaction.data.cryptoUpdateAccount.accountIDToUpdate.account
            .accountNum);
}

void reformat_unstake_account_to_update(void) {
    set_amount_title("Unstake account");

    hedera_safe_printf(
        st_ctx.amount, "%llu.%llu.%llu",
        st_ctx.transaction.data.cryptoUpdateAccount.accountIDToUpdate.shardNum,
        st_ctx.transaction.data.cryptoUpdateAccount.accountIDToUpdate.realmNum,
        st_ctx.transaction.data.cryptoUpdateAccount.accountIDToUpdate.account
            .accountNum);
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
    set_amount_title("Amount");
    uint64_t amount = st_ctx.transaction.data.cryptoTransfer.tokenTransfers[0]
                          .transfers[st_ctx.transfer_to_index]
                          .amount;
    uint32_t decimals = st_ctx.transaction.data.cryptoTransfer.tokenTransfers[0]
                            .expected_decimals.value;
    validate_decimals(decimals);
    if (st_ctx.token_known) {
        hedera_safe_printf(st_ctx.amount, "%s %s",
                           hedera_format_amount(amount, st_ctx.token_decimals),
                           st_ctx.token_ticker);
    } else {
        hedera_safe_printf(st_ctx.amount, "%s",
                           hedera_format_amount(amount, decimals));
    }
}

// FEE

void reformat_fee(void) {
#if defined(TARGET_NANOS)
    set_title("Max fees");
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

// CRYPTO UPDATE specific fields

void reformat_auto_renew_period(void) {
    if (st_ctx.type == Update &&
        st_ctx.transaction.data.cryptoUpdateAccount.has_autoRenewPeriod) {
        uint64_t seconds =
            st_ctx.transaction.data.cryptoUpdateAccount.autoRenewPeriod.seconds;
            
        format_time_duration(st_ctx.auto_renew_period, sizeof(st_ctx.auto_renew_period), seconds);
    } else {
        hedera_safe_printf(st_ctx.auto_renew_period, "-");
    }
}

void reformat_expiration_time(void) {
    if (st_ctx.type == Update &&
        st_ctx.transaction.data.cryptoUpdateAccount.has_expirationTime) {
        // Show raw timestamp (Unix seconds since epoch)
        hedera_safe_printf(
            st_ctx.expiration_time, "%llu",
            st_ctx.transaction.data.cryptoUpdateAccount.expirationTime.seconds);
    } else {
        hedera_safe_printf(st_ctx.expiration_time, "-");
    }
}

void reformat_receiver_sig_required(void) {
    // Early return for non-Update transactions
    if (st_ctx.type != Update) {
        hedera_safe_printf(st_ctx.receiver_sig_required, "-");
        return;
    }
    
    // Check if receiver sig required field is not set
    if (st_ctx.transaction.data.cryptoUpdateAccount.which_receiverSigRequiredField !=
        Hedera_CryptoUpdateTransactionBody_receiverSigRequiredWrapper_tag) {
        hedera_safe_printf(st_ctx.receiver_sig_required, "-");
        return;
    }
    
    // Field is set, get the value and format accordingly
    bool required = st_ctx.transaction.data.cryptoUpdateAccount
                        .receiverSigRequiredField.receiverSigRequiredWrapper.value;
    
    hedera_safe_printf(st_ctx.receiver_sig_required, required ? "Yes" : "No");
}

void reformat_max_automatic_token_associations(void) {
    if (st_ctx.type == Update && st_ctx.transaction.data.cryptoUpdateAccount
                                     .has_max_automatic_token_associations) {
        hedera_safe_printf(st_ctx.max_auto_token_assoc, "%d",
                           st_ctx.transaction.data.cryptoUpdateAccount
                               .max_automatic_token_associations.value);
    } else {
        hedera_safe_printf(st_ctx.max_auto_token_assoc, "-");
    }
}

void reformat_collect_rewards_in_stake_flow(void) {
    bool declineRewards =
        st_ctx.transaction.data.cryptoCreateAccount.decline_reward;
    hedera_safe_printf(st_ctx.collect_rewards, "%s",
                       !declineRewards ? "yes" : "no");
}