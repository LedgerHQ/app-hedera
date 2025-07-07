from typing import Dict

from proto import transaction_body_pb2
from proto import basic_types_pb2
from proto import crypto_create_pb2
from proto import crypto_update_pb2
from proto import token_associate_pb2
from proto import token_dissociate_pb2
from proto import crypto_transfer_pb2
from proto import token_burn_pb2
from proto import token_mint_pb2
from proto import wrappers_pb2
from proto import timestamp_pb2
from proto import duration_pb2


def hedera_transaction(
    operator_shard_num: int,
    operator_realm_num: int,
    operator_account_num: int,
    transaction_fee: int,
    memo: str,
    conf: Dict,
) -> bytes:
    operator = basic_types_pb2.AccountID(
        shardNum=operator_shard_num,
        realmNum=operator_realm_num,
        accountNum=operator_account_num,
    )

    hedera_transaction_id = basic_types_pb2.TransactionID(accountID=operator)

    transaction = transaction_body_pb2.TransactionBody(
        transactionID=hedera_transaction_id,
        transactionFee=transaction_fee,
        memo=memo,
        **conf
    )

    return transaction.SerializeToString()


def crypto_create_account_conf(
    initialBalance: int,
    stakeTargetAccount: int = None,
    stakeTargetNode: int = None,
    declineRewards: bool = False,
) -> Dict:
    crypto_create_account = crypto_create_pb2.CryptoCreateTransactionBody(
        initialBalance=initialBalance
    )

    if stakeTargetAccount:
        account_id = basic_types_pb2.AccountID(
            shardNum=0, realmNum=0, accountNum=stakeTargetAccount
        )
        crypto_create_account = crypto_create_pb2.CryptoCreateTransactionBody(
            initialBalance=initialBalance,
            staked_account_id=account_id,
            decline_reward=declineRewards,
        )
    elif stakeTargetNode:
        crypto_create_account = crypto_create_pb2.CryptoCreateTransactionBody(
            initialBalance=initialBalance,
            staked_node_id=stakeTargetNode,
            decline_reward=declineRewards,
        )

    return {"cryptoCreateAccount": crypto_create_account}


def crypto_update_account_conf(
    declineRewards: bool = None,
    stakeTargetNode: int = None,
    targetShardNum: int = 0,
    targetRealmNum: int = 0,
    targetAccountNum: int = 666,
    autoRenewPeriodSeconds: int = None,
    expirationTimeSeconds: int = None,
    receiverSigRequired: bool = None,
    maxAutoTokenAssociations: int = None,
    includeKey: bool = False,
    accountMemo: str = None,
    stakeTargetShardNum: int = None,
    stakeTargetRealmNum: int = None,
    stakeTargetAccount: int = None,
) -> Dict:
    account_id = basic_types_pb2.AccountID(
        shardNum=targetShardNum, realmNum=targetRealmNum, accountNum=targetAccountNum
    )
    
    crypto_update_account = crypto_update_pb2.CryptoUpdateTransactionBody(
        accountIDToUpdate=account_id
    )
    
    # Add key if requested (for testing key change failure)
    if includeKey:
        dummy_key = basic_types_pb2.Key()
        crypto_update_account.key.CopyFrom(dummy_key)
    
    # Add decline rewards if specified
    if declineRewards is not None:
        decline = wrappers_pb2.BoolValue(value=declineRewards)
        crypto_update_account.decline_reward.CopyFrom(decline)
    
    # Add staking configuration
    if stakeTargetAccount is not None:
        stake_account_id = basic_types_pb2.AccountID(
            shardNum=stakeTargetShardNum,
            realmNum=stakeTargetRealmNum,
            accountNum=stakeTargetAccount,
        )
        crypto_update_account.staked_account_id.CopyFrom(stake_account_id)
    elif stakeTargetNode is not None:
        crypto_update_account.staked_node_id = stakeTargetNode
    
    # Add auto renew period if specified
    if autoRenewPeriodSeconds is not None:
        auto_renew_period = duration_pb2.Duration(seconds=autoRenewPeriodSeconds)
        crypto_update_account.autoRenewPeriod.CopyFrom(auto_renew_period)
    
    # Add expiration time if specified
    if expirationTimeSeconds is not None:
        expiration_time = timestamp_pb2.Timestamp(seconds=expirationTimeSeconds)
        crypto_update_account.expirationTime.CopyFrom(expiration_time)
    
    # Add receiver signature required if specified
    if receiverSigRequired is not None:
        receiver_sig = wrappers_pb2.BoolValue(value=receiverSigRequired)
        crypto_update_account.receiverSigRequiredWrapper.CopyFrom(receiver_sig)
    
    # Add max automatic token associations if specified
    if maxAutoTokenAssociations is not None:
        max_tokens = wrappers_pb2.Int32Value(value=maxAutoTokenAssociations)
        crypto_update_account.max_automatic_token_associations.CopyFrom(max_tokens)

    if accountMemo is not None:
        account_memo_value = wrappers_pb2.StringValue(value=accountMemo)
        crypto_update_account.memo.CopyFrom(account_memo_value)

    return {"cryptoUpdateAccount": crypto_update_account}


def crypto_transfer_token_conf(
    token_shardNum: int,
    token_realmNum: int,
    token_tokenNum: int,
    sender_shardNum: int,
    sender_realmNum: int,
    sender_accountNum: int,
    recipient_shardNum: int,
    recipient_realmNum: int,
    recipient_accountNum: int,
    amount: int,
    decimals: int,
) -> Dict:
    hedera_token_id = basic_types_pb2.TokenID(
        shardNum=token_shardNum,
        realmNum=token_realmNum,
        tokenNum=token_tokenNum,
    )

    hedera_account_id_sender = basic_types_pb2.AccountID(
        shardNum=sender_shardNum,
        realmNum=sender_realmNum,
        accountNum=sender_accountNum,
    )

    hedera_account_amount_sender = basic_types_pb2.AccountAmount(
        accountID=hedera_account_id_sender,
        amount=0,
    )

    hedera_account_id_recipient = basic_types_pb2.AccountID(
        shardNum=recipient_shardNum,
        realmNum=recipient_realmNum,
        accountNum=recipient_accountNum,
    )

    hedera_account_amount_recipient = basic_types_pb2.AccountAmount(
        accountID=hedera_account_id_recipient,
        amount=amount,
    )

    hedera_transfer_list = basic_types_pb2.TransferList(
        accountAmounts=[],
    )

    decimalsUInt32 = wrappers_pb2.UInt32Value(
        value=decimals,
    )

    hedera_token_transfer_list = basic_types_pb2.TokenTransferList(
        token=hedera_token_id,
        transfers=[hedera_account_amount_recipient, hedera_account_amount_sender],
        expected_decimals=decimalsUInt32,
    )

    crypto_transfer = crypto_transfer_pb2.CryptoTransferTransactionBody(
        transfers=hedera_transfer_list,
        tokenTransfers=[hedera_token_transfer_list],
    )

    return {"cryptoTransfer": crypto_transfer}


def crypto_transfer_hbar_conf(
    sender_shardNum: int,
    sender_realmNum: int,
    sender_accountNum: int,
    recipient_shardNum: int,
    recipient_realmNum: int,
    recipient_accountNum: int,
    amount: int,
) -> Dict:

    hedera_account_id_sender = basic_types_pb2.AccountID(
        shardNum=sender_shardNum,
        realmNum=sender_realmNum,
        accountNum=sender_accountNum,
    )

    hedera_account_amount_sender = basic_types_pb2.AccountAmount(
        accountID=hedera_account_id_sender,
        amount=0,
    )

    hedera_account_id_recipient = basic_types_pb2.AccountID(
        shardNum=recipient_shardNum,
        realmNum=recipient_realmNum,
        accountNum=recipient_accountNum,
    )

    hedera_account_amount_recipient = basic_types_pb2.AccountAmount(
        accountID=hedera_account_id_recipient,
        amount=amount,
    )

    hedera_transfer_list = basic_types_pb2.TransferList(
        accountAmounts=[hedera_account_amount_recipient, hedera_account_amount_sender],
    )

    crypto_transfer = crypto_transfer_pb2.CryptoTransferTransactionBody(
        transfers=hedera_transfer_list,
        tokenTransfers=[],
    )

    return {"cryptoTransfer": crypto_transfer}


def crypto_transfer_verify(
    sender_shardNum: int, sender_realmNum: int, sender_accountNum: int
) -> Dict:

    hedera_account_id_sender = basic_types_pb2.AccountID(
        shardNum=sender_shardNum,
        realmNum=sender_realmNum,
        accountNum=sender_accountNum,
    )

    hedera_account_amount_sender = basic_types_pb2.AccountAmount(
        accountID=hedera_account_id_sender,
        amount=0,
    )

    hedera_transfer_list = basic_types_pb2.TransferList(
        accountAmounts=[hedera_account_amount_sender],
    )

    crypto_transfer = crypto_transfer_pb2.CryptoTransferTransactionBody(
        transfers=hedera_transfer_list,
        tokenTransfers=[],
    )

    return {"cryptoTransfer": crypto_transfer}


def token_associate_conf(
    token_shardNum: int,
    token_realmNum: int,
    token_tokenNum: int,
    sender_shardNum: int,
    sender_realmNum: int,
    sender_accountNum: int,
) -> Dict:
    hedera_account_id_sender = basic_types_pb2.AccountID(
        shardNum=sender_shardNum,
        realmNum=sender_realmNum,
        accountNum=sender_accountNum,
    )

    hedera_token_id = basic_types_pb2.TokenID(
        shardNum=token_shardNum,
        realmNum=token_realmNum,
        tokenNum=token_tokenNum,
    )

    token_associate = token_associate_pb2.TokenAssociateTransactionBody(
        account=hedera_account_id_sender,
        tokens=[hedera_token_id],
    )

    return {"tokenAssociate": token_associate}


def token_dissociate_conf(
    token_shardNum: int,
    token_realmNum: int,
    token_tokenNum: int,
    sender_shardNum: int,
    sender_realmNum: int,
    sender_accountNum: int,
) -> Dict:
    hedera_account_id_sender = basic_types_pb2.AccountID(
        shardNum=sender_shardNum,
        realmNum=sender_realmNum,
        accountNum=sender_accountNum,
    )

    hedera_token_id = basic_types_pb2.TokenID(
        shardNum=token_shardNum,
        realmNum=token_realmNum,
        tokenNum=token_tokenNum,
    )

    token_associate = token_dissociate_pb2.TokenDissociateTransactionBody(
        account=hedera_account_id_sender,
        tokens=[hedera_token_id],
    )

    return {"tokenDissociate": token_associate}


def token_burn_conf(
    token_shardNum: int, token_realmNum: int, token_tokenNum: int, amount: int
) -> Dict:
    hedera_token_id = basic_types_pb2.TokenID(
        shardNum=token_shardNum,
        realmNum=token_realmNum,
        tokenNum=token_tokenNum,
    )

    token_burn = token_burn_pb2.TokenBurnTransactionBody(
        token=hedera_token_id, amount=amount
    )

    return {"tokenBurn": token_burn}


def token_mint_conf(
    token_shardNum: int,
    token_realmNum: int,
    token_tokenNum: int,
    amount: int,
) -> Dict:
    hedera_token_id = basic_types_pb2.TokenID(
        shardNum=token_shardNum,
        realmNum=token_realmNum,
        tokenNum=token_tokenNum,
    )

    token_mint = token_mint_pb2.TokenMintTransactionBody(
        token=hedera_token_id,
        amount=amount,
    )

    return {"tokenMint": token_mint}
