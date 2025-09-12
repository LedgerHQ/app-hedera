#include <stdbool.h>
#include "staking.h"

update_type_t identify_special_update(const struct _Hedera_CryptoUpdateTransactionBody *update_body) {
    (void)update_body;
    return GENERIC_UPDATE;
}

bool is_ledger_account(const Hedera_AccountID *account_id) {
    (void)account_id;
    return false;
}


