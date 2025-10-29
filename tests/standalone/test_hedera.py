from ragger.backend.interface import RAPDU, RaisePolicy
from ragger.navigator import NavInsID
from ragger.firmware import Firmware
from ragger.firmware.touch.use_cases import UseCaseReview
import pytest

from tests.application_client.hedera import HederaClient, ErrorType, STATUS_OK
from tests.application_client.hedera_builder import crypto_create_account_conf
from tests.application_client.hedera_builder import crypto_update_account_conf
from tests.application_client.hedera_builder import crypto_transfer_token_conf
from tests.application_client.hedera_builder import crypto_transfer_hbar_conf
from tests.application_client.hedera_builder import crypto_transfer_verify
from tests.application_client.hedera_builder import token_associate_conf
from tests.application_client.hedera_builder import token_dissociate_conf
from tests.application_client.hedera_builder import token_burn_conf
from tests.application_client.hedera_builder import token_mint_conf
from tests.application_client.hedera_builder import contract_call_conf, hedera_transaction
from tests.application_client.hedera_builder import encode_erc20_transfer_web3, encode_erc20_with_wrong_selector

from .utils import (ROOT_SCREENSHOT_PATH,
                    navigation_helper_confirm,
                    navigation_helper_reject,
                    navigate_erc20_confirm,
                    navigate_erc20_reject,
                    navigate_erc20_reject_at_warning, 
                    navigate_erc20_show_qr_code)


def test_hedera_get_public_key_ok(backend, firmware, navigator, test_name):
    hedera = HederaClient(backend)
    values = [
        (0, "78be747e6894ee5f965e3fb0e4c1628af2f9ae0d94dc01d9b9aab75484c3184b"),
        (11095, "644ef690d394e8140fa278273913425bc83c59067a392a9e7f703ead4973caf8"),
        (294967295, "02357008e57f96bb250f789c63eb3a241c1eae034d461468b76b8174a59bdc9b"),
        (
            2294967295,
            "2cbd40ac0a3e25a315aed7e211fd0056127075dfa4ba1717a7a047a2030b5efb",
        ),
    ]
    for i, (index, key) in enumerate(values):
        from_public_key = hedera.get_public_key_non_confirm(index).data
        backend.wait_for_home_screen()
        assert from_public_key.hex() == key
        with hedera.get_public_key_confirm(index):
            if firmware.device == "nanos":
                nav_ins = [NavInsID.RIGHT_CLICK]
            elif backend.firmware.device.startswith("nano"):
                nav_ins = [NavInsID.RIGHT_CLICK,
                           NavInsID.BOTH_CLICK]
            else:
                nav_ins = [NavInsID.USE_CASE_REVIEW_NEXT,
                           NavInsID.USE_CASE_ADDRESS_CONFIRMATION_CONFIRM]
            navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH, test_name + "_" + str(i), nav_ins)

        from_public_key = hedera.get_async_response().data
        assert from_public_key.hex() == key


def test_hedera_get_public_key_refused(backend, firmware, navigator, test_name):
    hedera = HederaClient(backend)
    with hedera.get_public_key_confirm(0):
        if firmware.device == "nanos":
            nav_ins = [NavInsID.LEFT_CLICK]
        elif backend.firmware.device.startswith("nano"):
            nav_ins = [NavInsID.RIGHT_CLICK,
                       NavInsID.RIGHT_CLICK,
                       NavInsID.BOTH_CLICK]
        else:
            nav_ins = [NavInsID.USE_CASE_REVIEW_NEXT,
                           NavInsID.USE_CASE_ADDRESS_CONFIRMATION_CANCEL]
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH, test_name, nav_ins)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED

    if not firmware.is_nano:
        with hedera.get_public_key_confirm(0):
            backend.raise_policy = RaisePolicy.RAISE_NOTHING
            nav_ins = [NavInsID.USE_CASE_REVIEW_NEXT,
                       NavInsID.USE_CASE_ADDRESS_CONFIRMATION_CANCEL]
            navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH, test_name + "_2", nav_ins)

        rapdu = hedera.get_async_response()
        assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_crypto_create_account_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_create_account_conf(initialBalance=5)
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)


def test_hedera_crypto_create_account_refused(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_create_account_conf(initialBalance=5)
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        navigation_helper_reject(firmware, scenario_navigator)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_crypto_create_account_stake_account_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_create_account_conf(
        initialBalance=5, stakeTargetAccount=666, declineRewards=True
    )
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)


def test_hedera_crypto_create_account_stake_account_refused(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_create_account_conf(
        initialBalance=5, stakeTargetAccount=777, declineRewards=False
    )
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        navigation_helper_reject(firmware, scenario_navigator)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_crypto_create_account_stake_node_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_create_account_conf(
        initialBalance=5, stakeTargetNode=4, declineRewards=True
    )
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)


def test_hedera_crypto_create_account_stake_node_refused(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_create_account_conf(
        initialBalance=5, stakeTargetNode=3, declineRewards=False
    )
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        navigation_helper_reject(firmware, scenario_navigator)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_crypto_create_account_stake_ledger_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_create_account_conf(
        initialBalance=5, stakeTargetAccount=1337, declineRewards=True
    )
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)


def test_hedera_crypto_create_account_stake_ledger_refused(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_create_account_conf(
        initialBalance=5, stakeTargetAccount=1337, declineRewards=False
    )
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        navigation_helper_reject(firmware, scenario_navigator)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_crypto_update_account_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_update_account_conf(
        targetShardNum=6, targetRealmNum=54, targetAccountNum=6789
    )
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)


def test_hedera_crypto_update_account_refused(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_update_account_conf(
        targetShardNum=6, targetRealmNum=54, targetAccountNum=6789
    )
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        if firmware is Firmware.NANOS:
            scenario_navigator.review_reject(custom_screen_text="Deny")
        elif firmware.is_nano:
            scenario_navigator.review_reject(custom_screen_text="Reject")
        else:
            navigation_helper_reject(firmware, scenario_navigator)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_crypto_update_account_stake_account_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_update_account_conf(
        targetShardNum=8,
        targetRealmNum=901,
        targetAccountNum=7,
        stakeTargetAccount=666,
        stakeTargetShardNum=555,
        stakeTargetRealmNum=444,
        declineRewards=True,
    )
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)


def test_hedera_crypto_update_account_stake_account_refused(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_update_account_conf(
        targetShardNum=8,
        targetRealmNum=901,
        targetAccountNum=7,
        stakeTargetAccount=777,
        declineRewards=False,
    )
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        navigation_helper_reject(firmware, scenario_navigator)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_crypto_update_account_stake_ledger_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_update_account_conf(
        targetShardNum=8,
        targetRealmNum=901,
        targetAccountNum=7,
        stakeTargetAccount=1337,
        stakeTargetShardNum=0,
        stakeTargetRealmNum=0,
        declineRewards=True,
    )
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)


def test_hedera_crypto_update_account_stake_ledger_refused(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_update_account_conf(
        targetShardNum=8,
        targetRealmNum=901,
        targetAccountNum=7,
        stakeTargetAccount=1337,
        stakeTargetShardNum=0,
        stakeTargetRealmNum=0,
        declineRewards=False,
    )
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        navigation_helper_reject(firmware, scenario_navigator)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_crypto_update_account_stake_node_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_update_account_conf(
        targetShardNum=87,
        targetRealmNum=4,
        targetAccountNum=343434,
        stakeTargetNode=4,
        declineRewards=True,
    )
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)

def test_hedera_crypto_transfer_collect_rewards_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    # Create specilized transaction for collecting rewards
    # Receipent => 0.0.800
    # Amount => 1 tinybar
    # Memo => Collect Staking Rewards
    conf = crypto_transfer_hbar_conf(
        sender_shardNum=0,
        sender_realmNum=0,
        sender_accountNum=12345,
        recipient_shardNum=0,
        recipient_realmNum=0,
        recipient_accountNum=800,
        amount=1, # 1 tinybar 
    )
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=0,
        operator_realm_num=0,
        operator_account_num=12345,
        transaction_fee=5,
        memo="Collect Staking Rewards",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)

def test_hedera_crypto_update_account_stake_node_refused(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_update_account_conf(
        targetShardNum=87,
        targetRealmNum=4,
        targetAccountNum=343434,
        stakeTargetNode=3,
        declineRewards=False,
    )
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        navigation_helper_reject(firmware, scenario_navigator)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_crypto_update_account_unstake_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_update_account_conf(
        targetShardNum=0,
        targetRealmNum=0,
        targetAccountNum=654321,
        stakeTargetShardNum=0,
        stakeTargetRealmNum=0,
        stakeTargetAccount=0,
        declineRewards=False,
    )
    with hedera.send_sign_transaction(
        index=1,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=int(10**8), # 1 Hbar
        memo="Unstaking Hbar",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)

def test_hedera_crypto_update_account_unstake_node_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_update_account_conf(
        targetShardNum=0,
        targetRealmNum=0,
        targetAccountNum=654321,
        stakeTargetNode=-1,
        declineRewards=False,
    )
    with hedera.send_sign_transaction(
        index=1,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=int(10**8), # 1 Hbar
        memo="Unstaking Hbar",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)

def test_hedera_crypto_update_account_unstake_node_refused(backend, firmware, scenario_navigator):

    hedera = HederaClient(backend)
    conf = crypto_update_account_conf(
        targetShardNum=0,
        targetRealmNum=0,
        targetAccountNum=654321,
        stakeTargetNode=-1,
        declineRewards=False,
    )
    with hedera.send_sign_transaction(
        index=1,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=int(10**8), # 1 Hbar
        memo="Unstaking Hbar",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        navigation_helper_reject(firmware, scenario_navigator)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED

def test_hedera_crypto_update_account_invalid_zero_account(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_update_account_conf(
        targetShardNum=0, targetRealmNum=0, targetAccountNum=0
    )
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        #navigation_helper_reject(firmware, scenario_navigator)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_MALFORMED_APDU



def test_hedera_crypto_update_account_key_change_refused(backend, firmware, scenario_navigator):
    """Test that crypto update fails when trying to change the key"""
    hedera = HederaClient(backend)
    conf = crypto_update_account_conf(
        targetShardNum=1, 
        targetRealmNum=2, 
        targetAccountNum=3,
        includeKey=True
    )
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="key_change_test",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_MALFORMED_APDU



def test_hedera_crypto_update_account_auto_renew_period_formatting_1(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_update_account_conf(
        targetShardNum=5,
        targetRealmNum=10,
        targetAccountNum=12345,
        autoRenewPeriodSeconds=134, # 134 seconds
    )
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=10,
        memo="period: 134 seconds",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)

def test_hedera_crypto_update_account_auto_renew_period_formatting_2(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_update_account_conf(
        targetShardNum=5,
        targetRealmNum=10,
        targetAccountNum=12345,
        autoRenewPeriodSeconds=86400, # 1 day
    )
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=10,
        memo="period: 1 day",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)

def test_hedera_crypto_update_account_auto_renew_period_formatting_long(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_update_account_conf(
        targetShardNum=5,
        targetRealmNum=10,
        targetAccountNum=12345,
        autoRenewPeriodSeconds=0x7FFFFFFFFFFFFFFF, # 106751991167300 days 55807 seconds
    )
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=10,
        memo="long time period",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)
        
        

def test_hedera_crypto_update_account_all_fields_ok(backend, firmware, scenario_navigator):
    """Test crypto update with all possible fields set"""
    hedera = HederaClient(backend)
    conf = crypto_update_account_conf(
        targetShardNum=5,
        targetRealmNum=10,
        targetAccountNum=12345,
        autoRenewPeriodSeconds=7776000,  # 90 days
        expirationTimeSeconds=1735689600,  # Jan 1, 2025
        receiverSigRequired=True,
        maxAutoTokenAssociations=100,
        stakeTargetNode=2,
        declineRewards=False,
        accountMemo="Nice Account Memo",
    )
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=10,
        memo="comprehensive_update_test",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)



def test_hedera_transfer_token_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_transfer_token_conf(
        token_shardNum=15,
        token_realmNum=16,
        token_tokenNum=17,
        sender_shardNum=57,
        sender_realmNum=58,
        sender_accountNum=59,
        recipient_shardNum=100,
        recipient_realmNum=101,
        recipient_accountNum=102,
        amount=1234567890,
        decimals=9,
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)


def test_hedera_transfer_known_token_1_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_transfer_token_conf(
        token_shardNum=0,
        token_realmNum=0,
        token_tokenNum=5022567, #hBARK
        sender_shardNum=12312312345,
        sender_realmNum=0,
        sender_accountNum=0,
        recipient_shardNum=100,
        recipient_realmNum=101,
        recipient_accountNum=102,
        amount=1,
        decimals=0,
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="sending 1 hBARK",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)


def test_hedera_transfer_known_token_2_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_transfer_token_conf(
        token_shardNum=0, 
        token_realmNum=0,
        token_tokenNum=4794920,#PACK
        sender_shardNum=33,
        sender_realmNum=0,
        sender_accountNum=0,
        recipient_shardNum=100,
        recipient_realmNum=101,
        recipient_accountNum=102,
        amount=1,  #Should be 0.000001 PACK
        decimals=6,
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="sending 0.000001 PACK",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)


def test_hedera_transfer_max_supply_cal_token_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    # Max supply in Hedera is 2^63-1
    max_supply = (2**63) - 1
    
    # Use PACK token which is on the CAL list
    conf = crypto_transfer_token_conf(
        token_shardNum=0,
        token_realmNum=0,
        token_tokenNum=4794920, # PACK
        sender_shardNum=33,
        sender_realmNum=0,
        sender_accountNum=0,
        recipient_shardNum=100,
        recipient_realmNum=101,
        recipient_accountNum=102,
        amount=max_supply,
        decimals=6,
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="sending max supply PACK",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)


def test_hedera_transfer_max_supply_token_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    # Max supply in Hedera is 2^63-1
    max_supply = (2**63) - 1
    
    # Use a non-CAL token
    conf = crypto_transfer_token_conf(
        token_shardNum=15,
        token_realmNum=16,
        token_tokenNum=17,
        sender_shardNum=57,
        sender_realmNum=58,
        sender_accountNum=59,
        recipient_shardNum=100,
        recipient_realmNum=101,
        recipient_accountNum=102,
        amount=max_supply,
        decimals=9,
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="sending max supply token",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)


def test_hedera_transfer_longest_ticker_token_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    # Use a large number but not max supply
    large_amount = (2**63) - 1
    
    # Use HEDERA4TRUMP token which has the longest ticker in CAL
    conf = crypto_transfer_token_conf(
        token_shardNum=0,
        token_realmNum=0,
        token_tokenNum=8115822, # HEDERA4TRUMP
        sender_shardNum=42,
        sender_realmNum=0,
        sender_accountNum=999,
        recipient_shardNum=100,
        recipient_realmNum=101,
        recipient_accountNum=102,
        amount=large_amount,
        decimals=8,
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="sending large amount of HEDERA4TRUMP",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)


def test_hedera_transfer_known_token_sent_wrong_decimal(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_transfer_token_conf(
        token_shardNum=0,
        token_realmNum=0,
        token_tokenNum=4794920, #PACK
        sender_shardNum=33,
        sender_realmNum=0,
        sender_accountNum=0,
        recipient_shardNum=100,
        recipient_realmNum=101,
        recipient_accountNum=102,
        amount=int(12.34*(10**6)),  #Should be 12.34 PACK
        decimals=9, #Should be ignored
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="sending 12.34 PACK",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        navigation_helper_reject(firmware, scenario_navigator)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_transfer_token_refused(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_transfer_token_conf(
        token_shardNum=15,
        token_realmNum=16,
        token_tokenNum=17,
        sender_shardNum=57,
        sender_realmNum=58,
        sender_accountNum=59,
        recipient_shardNum=100,
        recipient_realmNum=101,
        recipient_accountNum=102,
        amount=1234567890,
        decimals=9,
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        navigation_helper_reject(firmware, scenario_navigator)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_transfer_hbar_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_transfer_hbar_conf(
        sender_shardNum=57,
        sender_realmNum=58,
        sender_accountNum=59,
        recipient_shardNum=100,
        recipient_realmNum=101,
        recipient_accountNum=102,
        amount=1234567890,
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)

def test_hedera_transfer_hbar_ok_with_key_index(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_transfer_hbar_conf(
        sender_shardNum=12345, sender_realmNum=12346, sender_accountNum=12347,
        recipient_shardNum=100,
        recipient_realmNum=101,
        recipient_accountNum=102,
        amount=int(12.23 * 10**8), # 12.23 Hbar
    )
    
    with hedera.send_sign_transaction(
        index=123,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=1,
        memo="Transaction with key 123",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)

def test_hedera_transfer_hbar_refused(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_transfer_hbar_conf(
        sender_shardNum=57,
        sender_realmNum=58,
        sender_accountNum=59,
        recipient_shardNum=100,
        recipient_realmNum=101,
        recipient_accountNum=102,
        amount=1234567890,
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        navigation_helper_reject(firmware, scenario_navigator)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_token_associate_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = token_associate_conf(
        token_shardNum=57,
        token_realmNum=58,
        token_tokenNum=59,
        sender_shardNum=100,
        sender_realmNum=101,
        sender_accountNum=102,
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)

def test_hedera_token_associate_known_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = token_associate_conf(
        token_shardNum=0,
        token_realmNum=0,
        token_tokenNum=7893707, #GIB
        sender_shardNum=1000,
        sender_realmNum=1001,
        sender_accountNum=1002,
    )

    with hedera.send_sign_transaction(
        index=123321,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=321,
        memo="this_is_the_memo",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)


def test_hedera_token_associate_refused(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = token_associate_conf(
        token_shardNum=57,
        token_realmNum=58,
        token_tokenNum=59,
        sender_shardNum=100,
        sender_realmNum=101,
        sender_accountNum=102,
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        navigation_helper_reject(firmware, scenario_navigator)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_token_dissociate_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = token_dissociate_conf(
        token_shardNum=57,
        token_realmNum=58,
        token_tokenNum=59,
        sender_shardNum=100,
        sender_realmNum=101,
        sender_accountNum=102,
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)


def test_hedera_token_dissociate_refused(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = token_dissociate_conf(
        token_shardNum=57,
        token_realmNum=58,
        token_tokenNum=59,
        sender_shardNum=100,
        sender_realmNum=101,
        sender_accountNum=102,
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        navigation_helper_reject(firmware, scenario_navigator)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_token_burn_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = token_burn_conf(
        token_shardNum=57, token_realmNum=58, token_tokenNum=59, amount=77
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)


def test_hedera_token_burn_refused(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = token_burn_conf(
        token_shardNum=57, token_realmNum=58, token_tokenNum=59, amount=77
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        navigation_helper_reject(firmware, scenario_navigator)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_token_mint_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = token_mint_conf(
        token_shardNum=57, token_realmNum=58, token_tokenNum=59, amount=77
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)


def test_hedera_token_mint_refused(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = token_mint_conf(
        token_shardNum=57, token_realmNum=58, token_tokenNum=59, amount=77
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="this_is_the_memo",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        navigation_helper_reject(firmware, scenario_navigator)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_transfer_verify_ok(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_transfer_verify(
        sender_shardNum=57, sender_realmNum=58, sender_accountNum=59
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=1,
        memo="this_is_the_memo",
        conf=conf,
    ):
        navigation_helper_confirm(firmware, scenario_navigator)


def test_hedera_transfer_verify_refused(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    conf = crypto_transfer_verify(
        sender_shardNum=57, sender_realmNum=58, sender_accountNum=59
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=1,
        memo="this_is_the_memo",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        navigation_helper_reject(firmware, scenario_navigator)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


# ================================
# ERC-20 tokens:
# ================================

def test_hedera_erc20_transfer_good_signature_contract_evm_address(backend, firmware, navigator, scenario_navigator, test_name):
    hedera = HederaClient(backend)

    # Random-like parameters (deterministic values for test stability)
    key_index = 54
    to_address = "123456789abcdef0112233445566778899aabbcc"
    amount = int("1"*4 + "2"*4 + "3"*4 + "4"*4 + "5"*4 + "6"*4 + "7"*4 + "8"*4 + "9"*4 + "0"*4)

    # Get the public key for verification (non-interactive)
    public_key = hedera.get_public_key_non_confirm(key_index).data
    backend.wait_for_home_screen()

    # Encode ERC-20 transfer payload
    params = encode_erc20_transfer_web3(to_address, amount)

    # Build contract call
    conf = contract_call_conf(
        gas=100000,
        amount=123456789,
        function_parameters=params,
        evm_address=bytes.fromhex("00112233445566778899aabbccddeeff00112233"),
    )

    # Sign using non-interactive flow: send_sign_transaction without navigation helpers
    with hedera.send_sign_transaction(
        index=key_index,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="ContractCall",
        conf=conf,
    ):
        navigate_erc20_confirm(firmware, navigator, scenario_navigator, ROOT_SCREENSHOT_PATH, test_name)

    signature = hedera.get_async_response().data

    # Rebuild the transaction body to verify signature
    transaction = hedera_transaction(
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="ContractCall",
        conf=conf,
    )

    # The device receives index+transaction but only signs the transaction part
    transaction_with_index = key_index.to_bytes(4, "little") + transaction

    assert hedera.verify_signature(public_key, transaction_with_index, signature)


def test_hedera_erc20_known_by_evm_address(backend, firmware, navigator, scenario_navigator, test_name):
    hedera = HederaClient(backend)

    # Use test token from CAL: amUSDC (Bonzo aToken USDC)
    key_index = 0
    to_address = "123456789abcdef0112233445566778899aabbcc"
    amount = 1233219 * 10**6 # 123.3219 amUSDC (6 decimals)

    params = encode_erc20_transfer_web3(to_address, amount)

    # EVM contract address from CAL entry (20 bytes)
    evm_addr = bytes.fromhex("b7687538c7f4cad022d5e97cc778d0b46457c5db")

    conf = contract_call_conf(
        gas=100000,
        amount=0,
        function_parameters=params,
        evm_address=evm_addr,
    )

    with hedera.send_sign_transaction(
        index=key_index,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="ERC20 amUSDC evm",
        conf=conf,
    ):
        navigate_erc20_confirm(firmware, navigator, scenario_navigator, ROOT_SCREENSHOT_PATH, test_name)

    rapdu = hedera.get_async_response()
    assert rapdu.status == STATUS_OK


def test_hedera_erc20_known_by_hedera_id(backend, firmware, navigator, scenario_navigator, test_name):
    hedera = HederaClient(backend)

    # Use test token from CAL: WETH
    key_index = 1
    to_address = "abcdefabcdefabcdefabcdefabcdefabcdefabcd"
    amount = 10**18 + 1 # 1.000000000000000001 WETH (18 decimals)

    params = encode_erc20_transfer_web3(to_address, amount)

    conf = contract_call_conf(
        gas=21000,
        amount=0,
        function_parameters=params,
        contract_shard_num=0,
        contract_realm_num=0,
        contract_num=9470869,
    )

    with hedera.send_sign_transaction(
        index=key_index,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="ERC20 known tokenID",
        conf=conf,
    ):
        navigate_erc20_confirm(firmware, navigator, scenario_navigator, ROOT_SCREENSHOT_PATH, test_name)

    rapdu = hedera.get_async_response()
    assert rapdu.status == STATUS_OK

def test_hedera_erc20_wrong_selector(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)

    # ERC-20 allowance selector (not supported): 0xdd62ed3e
    wrong_selector = 0xDD62ED3E
    to_address = "deadbeefdeadbeefcafebabefaceb00c12345678"
    amount = 543543543

    # Build payload with wrong selector
    params = encode_erc20_with_wrong_selector(to_address, amount, wrong_selector)

    # Build contract call and send without navigation; expect immediate APDU error
    conf = contract_call_conf(
        gas=100000,
        amount=123456789,
        function_parameters=params,
        contract_shard_num=1,
        contract_realm_num=2,
        contract_num=3,
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="ContractCall",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_MALFORMED_APDU

def test_hedera_send_transaction_wrong_length(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)

    with hedera.send_sign_transaction_wrong_length(1):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_MALFORMED_APDU

    with hedera.send_sign_transaction_wrong_length(4):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_MALFORMED_APDU


def test_hedera_erc20_transfer_good_signature_contract_id(backend, firmware, navigator, scenario_navigator, test_name):
    hedera = HederaClient(backend)

    key_index = 0
    to_address = "123456789abcdef0112233445566778899aabbcc"
    amount = 54354354332132213

    public_key = hedera.get_public_key_non_confirm(key_index).data
    backend.wait_for_home_screen()

    params = encode_erc20_transfer_web3(to_address, amount)

    conf = contract_call_conf(
        gas=100000,
        amount=123456789,
        function_parameters=params,
        contract_shard_num=1,
        contract_realm_num=2,
        contract_num=3456,
    )

    with hedera.send_sign_transaction(
        index=key_index,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="ContractCall",
        conf=conf,
    ):
        navigate_erc20_confirm(firmware, navigator, scenario_navigator, ROOT_SCREENSHOT_PATH, test_name)

    rapdu = hedera.get_async_response()
    assert rapdu.status == STATUS_OK
    signature = rapdu.data

    transaction = hedera_transaction(
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="ContractCall",
        conf=conf,
    )

    transaction_with_index = key_index.to_bytes(4, "little") + transaction

    assert hedera.verify_signature(public_key, transaction_with_index, signature)


def test_hedera_erc20_allow_nonzero_hbar_amount_in_token_transfer(backend, firmware, navigator, scenario_navigator, test_name):
    hedera = HederaClient(backend)

    key_index = 0
    to_address = "abcdefabcdefabcdefabcdefabcdefabcdefabcd"
    token_amount = 123456789  # any non-zero token transfer amount

    params = encode_erc20_transfer_web3(to_address, token_amount)

    # Non-zero HBAR amount should be allowed for ERC-20 transfer
    conf = contract_call_conf(
        gas=21000,
        amount=1,  # 1 tinybar
        function_parameters=params,
        evm_address=bytes.fromhex("00112233445566778899aabbccddeeff00112233"),
    )

    with hedera.send_sign_transaction(
        index=key_index,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="ContractCall",
        conf=conf,
    ):
        navigate_erc20_confirm(firmware, navigator, scenario_navigator, ROOT_SCREENSHOT_PATH, test_name)

    rapdu = hedera.get_async_response()
    assert rapdu.status == STATUS_OK
    signature = rapdu.data
    public_key = hedera.get_public_key_non_confirm(key_index).data
    transaction = hedera_transaction(
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="ContractCall",
        conf=conf,
    )
    transaction_with_index = key_index.to_bytes(4, "little") + transaction
    assert hedera.verify_signature(public_key, transaction_with_index, signature)


def test_hedera_erc20_allow_zero_to_address(backend, firmware, navigator, scenario_navigator, test_name):
    hedera = HederaClient(backend)

    key_index = 123
    to_address = "0" * 40  # 0x000...000
    token_amount = 1

    params = encode_erc20_transfer_web3(to_address, token_amount)

    conf = contract_call_conf(
        gas=100000,
        amount=0,
        function_parameters=params,
        contract_shard_num=0,
        contract_realm_num=0,
        contract_num=1234,
    )

    with hedera.send_sign_transaction(
        index=key_index,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="ERC20 zero to",
        conf=conf,
    ):
        navigate_erc20_confirm(firmware, navigator, scenario_navigator, ROOT_SCREENSHOT_PATH, test_name)

    rapdu = hedera.get_async_response()
    assert rapdu.status == STATUS_OK
    signature = rapdu.data
    public_key = hedera.get_public_key_non_confirm(key_index).data
    transaction = hedera_transaction(
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="ERC20 zero to",
        conf=conf,
    )
    transaction_with_index = key_index.to_bytes(4, "little") + transaction
    assert hedera.verify_signature(public_key, transaction_with_index, signature)

def test_hedera_erc20_allow_zero_token_amount(backend, firmware, navigator, scenario_navigator, test_name):
    hedera = HederaClient(backend)

    key_index = 321
    to_address = "deadbeefdeadbeefdeadbeefdeadbeefdeadbeef"
    token_amount = 0

    params = encode_erc20_transfer_web3(to_address, token_amount)

    conf = contract_call_conf(
        gas=100000,
        amount=0,
        function_parameters=params,
        contract_shard_num=1,
        contract_realm_num=2,
        contract_num=3,
    )

    with hedera.send_sign_transaction(
        index=key_index,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="ERC20 zero amount",
        conf=conf,
    ):
        navigate_erc20_confirm(firmware, navigator, scenario_navigator, ROOT_SCREENSHOT_PATH, test_name)

    rapdu = hedera.get_async_response()
    assert rapdu.status == STATUS_OK
    signature = rapdu.data
    public_key = hedera.get_public_key_non_confirm(key_index).data
    transaction = hedera_transaction(
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="ERC20 zero amount",
        conf=conf,
    )
    transaction_with_index = key_index.to_bytes(4, "little") + transaction
    assert hedera.verify_signature(public_key, transaction_with_index, signature)


def test_hedera_erc20_negative_gas_value(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)

    gas_value = -1  # minimal gas for display; ensure no trimming issues
    to_address = "abcdefabcdefabcdefabcdefabcdefabcdefabcd"
    token_amount = 1

    params = encode_erc20_transfer_web3(to_address, token_amount)

    conf = contract_call_conf(
        gas=gas_value,
        amount=0,
        function_parameters=params,
        contract_shard_num=1,
        contract_realm_num=2,
        contract_num=3,
    )

    with hedera.send_sign_transaction(
        index=321,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="Gas negative",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_MALFORMED_APDU

def test_hedera_erc20_negative_contract_value(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)

    to_address = "123456789abcdef0112233445566778899aabbcc"
    token_amount = 1000
    params = encode_erc20_transfer_web3(to_address, token_amount)
    conf = contract_call_conf(
        gas=100000,
        amount=-1,
        function_parameters=params,
        contract_shard_num=7,
        contract_realm_num=8,
        contract_num=9,
    )
    with hedera.send_sign_transaction(
        index=1337,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="Contract negative",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_MALFORMED_APDU


def test_hedera_erc20_negative_contract_id(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)
    to_address = "123456789abcdef0112233445566778899aabbcc"
    token_amount = 1000
    params = encode_erc20_transfer_web3(to_address, token_amount)
    conf = contract_call_conf(
        gas=100000,
        amount=0,
        function_parameters=params,
        contract_shard_num=7,
        contract_realm_num=8,
        contract_num=-9,
    )
    with hedera.send_sign_transaction(
        index=1337,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="Contract negative",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_MALFORMED_APDU

def test_hedera_erc20_token_amount_1000_raw_display(backend, firmware, navigator, scenario_navigator, test_name):
    hedera = HederaClient(backend)

    to_address = "123456789abcdef0112233445566778899aabbcc"
    token_amount = 1000
    params = encode_erc20_transfer_web3(to_address, token_amount)

    conf = contract_call_conf(
        gas=100000,
        amount=0,
        function_parameters=params,
        contract_shard_num=7,
        contract_realm_num=8,
        contract_num=9,
    )

    with hedera.send_sign_transaction(
        index=1337,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="Token 1000",
        conf=conf,
    ):
        navigate_erc20_confirm(firmware, navigator, scenario_navigator, ROOT_SCREENSHOT_PATH, test_name)


def test_hedera_erc20_token_amount_uint256_max_display_full(backend, firmware, navigator, scenario_navigator, test_name):
    hedera = HederaClient(backend)

    to_address = "feedfacecafebabedeadbeef0011223344556677"
    token_amount = (1 << 256) - 1
    params = encode_erc20_transfer_web3(to_address, token_amount)

    conf = contract_call_conf(
        gas=(1 << 63) - 1,  # int64 max
        amount=(1 << 63) - 1,  # int64 max
        function_parameters=params,
        contract_shard_num=(1 << 63) - 1,
        contract_realm_num=(1 << 63) - 1,
        contract_num=(1 << 63) - 1,
    )

    with hedera.send_sign_transaction(
        index=1337,
        operator_shard_num=(1 << 63) - 1,
        operator_realm_num=(1 << 63) - 1,
        operator_account_num=(1 << 63) - 1,
        transaction_fee=(1 << 64) - 1,
        memo="Token max",
        conf=conf,
    ):
        navigate_erc20_confirm(firmware, navigator, scenario_navigator, ROOT_SCREENSHOT_PATH, test_name)


def test_hedera_erc20_addresses_hex_full_display(backend, firmware, navigator, scenario_navigator, test_name):
    hedera = HederaClient(backend)

    # Patterned addresses to detect trimming
    to_address = ("12" * 19) + "31"  # 0x1212...1231
    params = encode_erc20_transfer_web3(to_address, 777)

    conf = contract_call_conf(
        gas=123456,
        amount=0,
        function_parameters=params,
        evm_address=bytes.fromhex(("ab" * 19) + "cd"),
    )

    with hedera.send_sign_transaction(
        index=1337,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="Addr full",
        conf=conf,
    ):
        navigate_erc20_confirm(firmware, navigator, scenario_navigator, ROOT_SCREENSHOT_PATH, test_name)


def test_hedera_erc20_reject_approve_selector(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)

    wrong_selector = 0x095EA7B3  # approve(address,uint256)
    to_address = "deadbeefdeadbeefcafebabefaceb00c12345678"
    amount = 123

    params = encode_erc20_with_wrong_selector(to_address, amount, wrong_selector)

    conf = contract_call_conf(
        gas=100000,
        amount=0,
        function_parameters=params,
        contract_shard_num=1,
        contract_realm_num=2,
        contract_num=3,
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="approve selector",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_MALFORMED_APDU


def test_hedera_erc20_reject_decimals_selector(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)

    wrong_selector = 0x313CE567  # decimals()
    to_address = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    amount = 0

    params = encode_erc20_with_wrong_selector(to_address, amount, wrong_selector)

    conf = contract_call_conf(
        gas=100000,
        amount=0,
        function_parameters=params,
        contract_shard_num=1,
        contract_realm_num=2,
        contract_num=3,
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="decimals selector",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_MALFORMED_APDU


def test_hedera_erc20_reject_last_param_too_short(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)

    to_address = "abcdefabcdefabcdefabcdefabcdefabcdefabcd"
    amount = 999
    base = encode_erc20_transfer_web3(to_address, amount)
    # Remove one trailing byte
    params = base[:-1]

    conf = contract_call_conf(
        gas=100000,
        amount=0,
        function_parameters=params,
        evm_address=bytes.fromhex("00112233445566778899aabbccddeeff00112233"),
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="param short",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_MALFORMED_APDU


def test_hedera_erc20_reject_last_param_too_long(backend, firmware, scenario_navigator):
    hedera = HederaClient(backend)

    to_address = "abcdefabcdefabcdefabcdefabcdefabcdefabcd"
    amount = 999
    base = encode_erc20_transfer_web3(to_address, amount)
    # Add one extra padding byte
    params = base + b"\x00"

    conf = contract_call_conf(
        gas=100000,
        amount=0,
        function_parameters=params,
        contract_shard_num=1,
        contract_realm_num=2,
        contract_num=3,
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="param long",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_MALFORMED_APDU


def test_hedera_erc20_user_rejects(backend, firmware, navigator, scenario_navigator, test_name):
    hedera = HederaClient(backend)

    to_address = "123456789abcdef0112233445566778899aabbcc"
    amount = 12345
    params = encode_erc20_transfer_web3(to_address, amount)

    conf = contract_call_conf(
        gas=100000,
        amount=0,
        function_parameters=params,
        contract_shard_num=1,
        contract_realm_num=2,
        contract_num=3,
    )

    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="User reject",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        navigate_erc20_reject(firmware, navigator, scenario_navigator, ROOT_SCREENSHOT_PATH, test_name)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_erc20_reject_warning(backend, firmware, navigator, scenario_navigator, test_name):
    hedera = HederaClient(backend)
    to_address = "123456789abcdef0112233445566778899aabbcc"
    token_amount = 1000
    params = encode_erc20_transfer_web3(to_address, token_amount)
    conf = contract_call_conf(
        gas=100000,
        amount=0,
        function_parameters=params,
        contract_shard_num=1,
        contract_realm_num=2,
        contract_num=3,
    )
    with hedera.send_sign_transaction(
        index=1337,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="ERC20 reject warning",
        conf=conf,
    ):
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        navigate_erc20_reject_at_warning(firmware, navigator, scenario_navigator, ROOT_SCREENSHOT_PATH, test_name)
    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_erc20_show_qr_code(backend, firmware, navigator, scenario_navigator, test_name):
    if firmware.is_nano:
        pytest.skip("QR codes are not implemented for nano")
        return

    hedera = HederaClient(backend)
    to_address = "123456789abcdef0112233445566778899aabbcc"
    token_amount = 1000
    params = encode_erc20_transfer_web3(to_address, token_amount)
    conf = contract_call_conf(
        gas=100000,
        amount=0,
        function_parameters=params,
        contract_shard_num=1,
        contract_realm_num=2,
        contract_num=3,
    )
    with hedera.send_sign_transaction(
        index=0,
        operator_shard_num=1,
        operator_realm_num=2,
        operator_account_num=3,
        transaction_fee=5,
        memo="Show QR code",
        conf=conf,
    ):
        navigate_erc20_show_qr_code(firmware, navigator, scenario_navigator, ROOT_SCREENSHOT_PATH, test_name)

    rapdu = hedera.get_async_response()
    assert rapdu.status == STATUS_OK
