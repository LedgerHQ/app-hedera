from typing import Dict
import json

from web3 import Web3
from proto import transaction_body_pb2
from proto import basic_types_pb2
from proto import crypto_create_pb2
from proto import crypto_update_pb2
from proto import token_associate_pb2
from proto import token_dissociate_pb2
from proto import crypto_transfer_pb2
from proto import token_burn_pb2
from proto import token_mint_pb2
from proto import contract_call_pb2
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


def contract_call_transaction(
    gas: int,
    amount: int,
    function_parameters: bytes = b"",
    contract_shard_num: int = None,
    contract_realm_num: int = None,
    contract_num: int = None,
    evm_address: bytes = None,
) -> bytes:
    """Create a contract call transaction.
    
    Args:
        gas: Gas limit for the contract call
        amount: Amount of tinybar to send with the call
        function_parameters: ABI-encoded function parameters
        contract_shard_num: Contract shard number (if using shard/realm/num format)
        contract_realm_num: Contract realm number (if using shard/realm/num format)
        contract_num: Contract number (if using shard/realm/num format)
        evm_address: 20-byte EVM address (alternative to shard/realm/num)
    
    Returns:
        Serialized contract call transaction body
    """
    if evm_address is not None:
        contract_id = basic_types_pb2.ContractID(
            shardNum=0,  # EVM addresses don't use shard/realm
            realmNum=0,
            evm_address=evm_address,
        )
    else:
        contract_id = basic_types_pb2.ContractID(
            shardNum=contract_shard_num,
            realmNum=contract_realm_num,
            contractNum=contract_num,
        )

    contract_call = contract_call_pb2.ContractCallTransactionBody(
        contractID=contract_id,
        gas=gas,
        amount=amount,
        functionParameters=function_parameters,
    )

    return contract_call.SerializeToString()


def contract_call_conf(
    gas: int,
    amount: int,
    function_parameters: bytes = b"",
    contract_shard_num: int = None,
    contract_realm_num: int = None,
    contract_num: int = None,
    evm_address: bytes = None,
) -> Dict:
    """Create a contract call configuration.
    
    Args:
        gas: Gas limit for the contract call
        amount: Amount of tinybar to send with the call
        function_parameters: ABI-encoded function parameters
        contract_shard_num: Contract shard number (if using shard/realm/num format)
        contract_realm_num: Contract realm number (if using shard/realm/num format)
        contract_num: Contract number (if using shard/realm/num format)
        evm_address: 20-byte EVM address (alternative to shard/realm/num)
    
    Returns:
        Dictionary with contract call transaction body
    """
    if evm_address is not None:
        contract_id = basic_types_pb2.ContractID(
            shardNum=0,  # EVM addresses don't use shard/realm
            realmNum=0,
            evm_address=evm_address,
        )
    else:
        contract_id = basic_types_pb2.ContractID(
            shardNum=contract_shard_num,
            realmNum=contract_realm_num,
            contractNum=contract_num,
        )

    contract_call = contract_call_pb2.ContractCallTransactionBody(
        contractID=contract_id,
        gas=gas,
        amount=amount,
        functionParameters=function_parameters,
    )

    return {"contractCall": contract_call}


# === Web3-based ERC-20 Encoding Functions ===

# ERC-20 ABI for transfer function
ERC20_ABI = [
    {
        "inputs": [
            {"internalType": "address", "name": "_to", "type": "address"},
            {"internalType": "uint256", "name": "_value", "type": "uint256"}
        ],
        "name": "transfer",
        "outputs": [{"internalType": "bool", "name": "success", "type": "bool"}],
        "stateMutability": "nonpayable",
        "type": "function"
    }
]

def encode_erc20_transfer_web3(to_address: str, amount: int) -> bytes:
    """Encode ERC-20 transfer using Web3 - same as Ethereum app pattern"""
    contract = Web3().eth.contract(abi=ERC20_ABI, address=None)
    # Convert string address to bytes if needed
    if isinstance(to_address, str):
        to_bytes = bytes.fromhex(to_address.replace('0x', ''))
    else:
        to_bytes = to_address
    # encode_abi returns hex string, convert to bytes
    hex_data = contract.encode_abi("transfer", [to_bytes, amount])
    return bytes.fromhex(hex_data[2:])  # Remove 0x prefix

def encode_erc20_with_wrong_selector(to_address: str, amount: int, wrong_selector: int) -> bytes:
    """Encode ERC-20 transfer with wrong selector for rejection testing"""
    # First encode normally
    correct_data = encode_erc20_transfer_web3(to_address, amount)
    # Replace first 4 bytes (selector) with wrong one
    wrong_selector_bytes = wrong_selector.to_bytes(4, 'big')
    return wrong_selector_bytes + correct_data[4:]

