#pragma once
#include "app_globals.h"
#include "app_io.h"
#include "sign_transaction.h"

/**
 * @brief Reformats the key.
 */
void reformat_key(void);

/**
 * @brief Reformats the operator.
 */
void reformat_operator(void);

/**
 * @brief Reformats a provided summary string.
 *
 * @param summary Pointer to the summary string to be reformatted.
 */
void reformat_summary(const char *summary);

/**
 * @brief Reformats information related to sending tokens.
 */
void reformat_summary_send_token(void);

/**
 * @brief Reformats stake target.
 */
void reformat_stake_target(void);

/**
 * @brief Reformats information related to collecting rewards.
 */
void reformat_collect_rewards(void);

/**
 * @brief Reformats amount balance.
 */
void reformat_amount_balance(void);

/**
 * @brief Reformats token associate.
 */
void reformat_token_associate(void);

/**
 * @brief Reformats token dissociation.
 */
void reformat_token_dissociate(void);

/**
 * @brief Reformats token burn.
 */
void reformat_token_burn(void);

/**
 * @brief Reformats token mint.
 */
void reformat_token_mint(void);

/**
 * @brief Reformats amount burn.
 */
void reformat_amount_burn(void);

/**
 * @brief Reformats amount mint.
 */
void reformat_amount_mint(void);

/**
 * @brief Reformats account verification.
 */
void reformat_verify_account(void);

/**
 * @brief Reformats sender account.
 */
void reformat_sender_account(void);

/**
 * @brief Reformats recipient account.
 */
void reformat_recipient_account(void);

void reformat_recipient_account(void);

/**
 * @brief Reformats token sender account.
 */
void reformat_token_sender_account(void);

/**
 * @brief Reformats token recipient account.
 */
void reformat_token_recipient_account(void);

/**
 * @brief Reformats updated account.
 */
void reformat_updated_account(void);

/**
 * @brief Reformats amount transfer.
 */
void reformat_amount_transfer(void);

/**
 * @brief Reformats token transfer.
 */
void reformat_token_transfer(void);

/**
 * @brief Reformats fee.
 */
void reformat_fee();

/**
 * @brief Reformats memo.
 */
void reformat_memo();
