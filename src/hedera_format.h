#pragma once
#include "globals.h"
#include "io.h"
#include "sign_transaction.h"

void reformat_key(void);

void reformat_operator(void);

void reformat_summary(const char *summary);

void reformat_summary_send_token(void);

void reformat_stake_target(void);

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

void reformat_updated_account(void);

void reformat_amount_transfer(void);

void reformat_token_transfer(void);

void reformat_fee();

void reformat_memo();