#include "staking.h"
#include "crypto_update.pb.h"
#include <stddef.h>

#ifndef DISABLE_LEDGER_STAKING_NODE
bool is_ledger_account(const Hedera_AccountID *account_id) {
    if (account_id == NULL) {
        return false;
    }
    
    return (account_id->shardNum == LEDGER_ACCOUNT_SHARD &&
            account_id->realmNum == LEDGER_ACCOUNT_REALM &&
            account_id->account.accountNum == LEDGER_ACCOUNT_NUM);
}
#endif

update_type_t identify_special_update(const Hedera_CryptoUpdateTransactionBody *update_body) {
    if (update_body == NULL) {
        return GENERIC_UPDATE;
    }

    // Check if any non-staking fields are set
    // If any of these are true, then non-staking fields are being modified
    
    // Key update (not allowed for staking-only)
    if (update_body->has_key) {
        return GENERIC_UPDATE;
    }
    
    // Proxy account (deprecated but still checked)
    if (update_body->has_proxyAccountID) {
        return GENERIC_UPDATE;
    }
    
    // Proxy fraction (deprecated but still checked)
    if (update_body->proxyFraction != 0) {
        return GENERIC_UPDATE;
    }
    
    // Send record threshold fields (deprecated)
    if (update_body->which_sendRecordThresholdField != 0) {
        return GENERIC_UPDATE;
    }
    
    // Receive record threshold fields (deprecated)
    if (update_body->which_receiveRecordThresholdField != 0) {
        return GENERIC_UPDATE;
    }
    
    // Auto renew period
    if (update_body->has_autoRenewPeriod) {
        return GENERIC_UPDATE;
    }
    
    // Expiration time
    if (update_body->has_expirationTime) {
        return GENERIC_UPDATE;
    }
    
    // Receiver signature required
    if (update_body->which_receiverSigRequiredField != 0) {
        return GENERIC_UPDATE;
    }
    
    // Account memo
    if (update_body->has_memo) {
        return GENERIC_UPDATE;
    }
    
    // Max automatic token associations
    if (update_body->has_max_automatic_token_associations) {
        return GENERIC_UPDATE;
    }
    
    // At this point, only staking fields can be set
    // Check what type of staking operation this is
    bool has_staking_target = false;
    bool is_unstake = false;
    
    // Check staking target (account or node)
    if (update_body->which_staked_id != 0) {
        has_staking_target = true;
        
        if (update_body->which_staked_id == Hedera_CryptoUpdateTransactionBody_staked_account_id_tag) {
            if (update_body->staked_id.staked_account_id.shardNum == 0 && 
                update_body->staked_id.staked_account_id.realmNum == 0 && 
                update_body->staked_id.staked_account_id.account.accountNum == 0) {
                is_unstake = true;
            }
        } else if (update_body->which_staked_id == Hedera_CryptoUpdateTransactionBody_staked_node_id_tag) {
            // Staking to a node - check if it's -1 (unstaking)
            if (update_body->staked_id.staked_node_id == -1) {
                is_unstake = true;
            }
        }
    }
    
    // Check decline rewards setting
    bool has_decline_reward = update_body->has_decline_reward;
    
    // Determine the update type based on what fields are set
    if (!has_staking_target && !has_decline_reward) {
        // No staking fields are being modified
        return GENERIC_UPDATE;
    }
    
    if (is_unstake) {
        return UNSTAKE_UPDATE;
    }
    
    return STAKE_UPDATE;
}
