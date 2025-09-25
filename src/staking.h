#pragma once

#include <stdbool.h>
#include "crypto_update.pb.h"

// Special Ledger account ID (currently disabled by feature flag DISABLE_LEDGER_STAKING_NODE)
#define LEDGER_ACCOUNT_SHARD 0
#define LEDGER_ACCOUNT_REALM 0
#define LEDGER_ACCOUNT_NUM 1337

typedef enum {
    GENERIC_UPDATE = 0,
    STAKE_UPDATE = 1,
    UNSTAKE_UPDATE = 2
} update_type_t;

/**
 * Check if a crypto update transaction only modifies staking fields
 * (decline_reward, staked_account_id, or staked_node_id)
 * 
 * @param update_body Pointer to the crypto update transaction body
 * @return update_type_t enum: GENERIC_UPDATE, STAKE_UPDATE, or UNSTAKE_UPDATE
 */
update_type_t identify_special_update(const struct _Hedera_CryptoUpdateTransactionBody *update_body);

#ifndef DISABLE_LEDGER_STAKING_NODE
/**
 * Check if an account ID matches the hardcoded Ledger account (0.0.1337)
 * 
 * @param account_id Pointer to the account ID to check
 * @return true if the account ID matches 0.0.1337, false otherwise
 */
bool is_ledger_account(const Hedera_AccountID *account_id);
#endif

