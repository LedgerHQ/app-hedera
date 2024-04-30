from ragger.backend.interface import RAPDU, RaisePolicy
from ragger.navigator import NavInsID

from .apps.hedera import HederaClient, ErrorType
from .apps.hedera_builder import crypto_create_account_conf
from .apps.hedera_builder import crypto_update_account_conf
from .apps.hedera_builder import crypto_transfer_token_conf
from .apps.hedera_builder import crypto_transfer_hbar_conf
from .apps.hedera_builder import crypto_transfer_verify
from .apps.hedera_builder import token_associate_conf
from .apps.hedera_builder import token_dissociate_conf
from .apps.hedera_builder import token_burn_conf
from .apps.hedera_builder import token_mint_conf

from .utils import ROOT_SCREENSHOT_PATH, navigation_helper_confirm, navigation_helper_reject


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
                nav_ins = [NavInsID.USE_CASE_CHOICE_CONFIRM,
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
            nav_ins = [NavInsID.USE_CASE_CHOICE_REJECT]
        backend.raise_policy = RaisePolicy.RAISE_NOTHING
        navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH, test_name, nav_ins)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED

    if firmware.device == "stax":
        with hedera.get_public_key_confirm(0):
            backend.raise_policy = RaisePolicy.RAISE_NOTHING
            nav_ins = [NavInsID.USE_CASE_CHOICE_CONFIRM,
                       NavInsID.USE_CASE_ADDRESS_CONFIRMATION_CANCEL]
            navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH, test_name + "_2", nav_ins)

        rapdu = hedera.get_async_response()
        assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_crypto_create_account_ok(backend, firmware, navigator, test_name):
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
        navigation_helper_confirm(navigator, firmware.device, test_name)


def test_hedera_crypto_create_account_refused(backend, firmware, navigator, test_name):
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
        navigation_helper_reject(navigator, firmware.device, test_name)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_crypto_create_account_stake_account_ok(backend, firmware, navigator, test_name):
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
        navigation_helper_confirm(navigator, firmware.device, test_name)


def test_hedera_crypto_create_account_stake_account_refused(backend, firmware, navigator, test_name):
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
        navigation_helper_reject(navigator, firmware.device, test_name)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_crypto_create_account_stake_node_ok(backend, firmware, navigator, test_name):
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
        navigation_helper_confirm(navigator, firmware.device, test_name)


def test_hedera_crypto_create_account_stake_node_refused(backend, firmware, navigator, test_name):
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
        navigation_helper_reject(navigator, firmware.device, test_name)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_crypto_update_account_ok(backend, firmware, navigator, test_name):
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
        navigation_helper_confirm(navigator, firmware.device, test_name)


def test_hedera_crypto_update_account_refused(backend, firmware, navigator, test_name):
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
        navigation_helper_reject(navigator, firmware.device, test_name)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_crypto_update_account_stake_account_ok(backend, firmware, navigator, test_name):
    hedera = HederaClient(backend)
    conf = crypto_update_account_conf(
        targetShardNum=8,
        targetRealmNum=901,
        targetAccountNum=7,
        stakeTargetAccount=666,
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
        navigation_helper_confirm(navigator, firmware.device, test_name)


def test_hedera_crypto_update_account_stake_account_refused(backend, firmware, navigator, test_name):
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
        navigation_helper_reject(navigator, firmware.device, test_name)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_crypto_update_account_stake_node_ok(backend, firmware, navigator, test_name):
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
        navigation_helper_confirm(navigator, firmware.device, test_name)


def test_hedera_crypto_update_account_stake_node_refused(backend, firmware, navigator, test_name):
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
        navigation_helper_reject(navigator, firmware.device, test_name)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_transfer_token_ok(backend, firmware, navigator, test_name):
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
        navigation_helper_confirm(navigator, firmware.device, test_name)


def test_hedera_transfer_token_refused(backend, firmware, navigator, test_name):
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
        navigation_helper_reject(navigator, firmware.device, test_name)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_transfer_hbar_ok(backend, firmware, navigator, test_name):
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
        navigation_helper_confirm(navigator, firmware.device, test_name)


def test_hedera_transfer_hbar_refused(backend, firmware, navigator, test_name):
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
        navigation_helper_reject(navigator, firmware.device, test_name)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_token_associate_ok(backend, firmware, navigator, test_name):
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
        navigation_helper_confirm(navigator, firmware.device, test_name)


def test_hedera_token_associate_refused(backend, firmware, navigator, test_name):
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
        navigation_helper_reject(navigator, firmware.device, test_name)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_token_dissociate_ok(backend, firmware, navigator, test_name):
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
        navigation_helper_confirm(navigator, firmware.device, test_name)


def test_hedera_token_dissociate_refused(backend, firmware, navigator, test_name):
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
        navigation_helper_reject(navigator, firmware.device, test_name)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_token_burn_ok(backend, firmware, navigator, test_name):
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
        navigation_helper_confirm(navigator, firmware.device, test_name)


def test_hedera_token_burn_refused(backend, firmware, navigator, test_name):
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
        navigation_helper_reject(navigator, firmware.device, test_name)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_token_mint_ok(backend, firmware, navigator, test_name):
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
        navigation_helper_confirm(navigator, firmware.device, test_name)


def test_hedera_token_mint_refused(backend, firmware, navigator, test_name):
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
        navigation_helper_reject(navigator, firmware.device, test_name)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED


def test_hedera_transfer_verify_ok(backend, firmware, navigator, test_name):
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
        navigation_helper_confirm(navigator, firmware.device, test_name)


def test_hedera_transfer_verify_refused(backend, firmware, navigator, test_name):
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
        navigation_helper_reject(navigator, firmware.device, test_name)

    rapdu = hedera.get_async_response()
    assert rapdu.status == ErrorType.EXCEPTION_USER_REJECTED
