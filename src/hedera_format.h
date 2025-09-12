#pragma once
#include "app_globals.h"
#ifndef NO_BOLOS_SDK
#include "app_io.h"
#else
#include "os.h"
#endif
#include "sign_transaction.h"
#include "tokens/token_address.h"
#include "printf.h"

void reformat_key(void);

void reformat_operator(void);

void reformat_summary(const char *summary);

void reformat_summary_send_token(void);

void reformat_summary_send_known_token(void);

void reformat_stake_target(void);

void reformat_stake_in_stake_flow(void);

void reformat_collect_rewards(void);

void reformat_amount_balance(void);

void reformat_token_associate(void);

void reformat_token_dissociate(void);

void reformat_token_burn(void);

void reformat_token_mint(void);

void reformat_amount_burn(void);

void reformat_amount_mint(void);

void reformat_verify_account(void);

void reformat_sender_account(void);

void reformat_recipient_account(void);

void reformat_token_sender_account(void);

void reformat_token_recipient_account(void);

void reformat_account_to_update(void);

void reformat_updated_account(void);

void reformat_unstake_account_to_update(void);

void reformat_amount_transfer(void);

void reformat_token_transfer(void);

void reformat_fee(void);

void address_to_string(const token_addr_t *addr, char *buf);

void reformat_memo(void);

void reformat_key_index(void);

void reformat_auto_renew_period(void);

void reformat_expiration_time(void);

void reformat_receiver_sig_required(void);

void reformat_max_automatic_token_associations(void);

void reformat_collect_rewards_in_stake_flow(void);

// Public helper to format tinybar as HBAR decimal string (e.g., 1.23456789)
// Returned pointer is to an internal static buffer; copy immediately if needed.
const char *hedera_format_tinybar_str(uint64_t tinybar);

#define hedera_safe_printf(element, ...) \
    hedera_snprintf(element, sizeof(element) - 1, __VA_ARGS__)
