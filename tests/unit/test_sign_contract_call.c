#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "mock/os.h"
#include "sign_contract_call.h"
#include "ui/app_globals.h"
#include "proto/contract_call.pb.h"

sign_tx_context_t st_ctx; // define global for unit test link
extern volatile unsigned int g_last_throw; // from mock/throw_mock.c

static void reset_ctx(void) {
    memset(&st_ctx, 0, sizeof(st_ctx));
}

static void test_contract_call_valid_evm_address(void **state) {
    (void)state;
    reset_ctx();
    Hedera_ContractCallTransactionBody tx = Hedera_ContractCallTransactionBody_init_zero;
    // selector a9059cbb
    tx.functionParameters.size = 4 + 32 + 32;
    tx.functionParameters.bytes[0] = 0xA9;
    tx.functionParameters.bytes[1] = 0x05;
    tx.functionParameters.bytes[2] = 0x9C;
    tx.functionParameters.bytes[3] = 0xBB;
    // address word
    for (int i = 0; i < 20; i++) tx.functionParameters.bytes[4 + 12 + i] = 0x33;
    // amount word -> 1
    tx.functionParameters.bytes[4 + 32 + 31] = 0x01;
    // contract ID as EVM address (20 bytes)
    tx.contractID.which_contract = Hedera_ContractID_evm_address_tag;
    tx.contractID.contract.evm_address.size = 20;
    for (int i = 0; i < 20; i++) tx.contractID.contract.evm_address.bytes[i] = 0x44;
    tx.gas = 123;
    tx.amount = 456;

    assert_true(validate_and_reformat_contract_call(&tx));
    // Check UI side-effects: senders (contract), recipients (to), amount (raw), gas
    assert_string_equal(st_ctx.senders, "0x4444444444444444444444444444444444444444");
    assert_string_equal(st_ctx.recipients, "0x3333333333333333333333333333333333333333");
    assert_string_not_equal(st_ctx.amount, "");
}

static void test_contract_call_invalid_params_length(void **state) {
    (void)state;
    reset_ctx();
    Hedera_ContractCallTransactionBody tx = Hedera_ContractCallTransactionBody_init_zero;
    tx.functionParameters.size = 4 + 32; // too short
    tx.functionParameters.bytes[0] = 0xA9;
    tx.functionParameters.bytes[1] = 0x05;
    tx.functionParameters.bytes[2] = 0x9C;
    tx.functionParameters.bytes[3] = 0xBB;
    assert_false(validate_and_reformat_contract_call(&tx));
}

static void test_contract_call_invalid_evm_address_size(void **state) {
    (void)state;
    reset_ctx();
    Hedera_ContractCallTransactionBody tx = Hedera_ContractCallTransactionBody_init_zero;
    tx.functionParameters.size = 4 + 32 + 32;
    tx.functionParameters.bytes[0] = 0xA9;
    tx.functionParameters.bytes[1] = 0x05;
    tx.functionParameters.bytes[2] = 0x9C;
    tx.functionParameters.bytes[3] = 0xBB;
    // Mark EVM address with wrong size
    tx.contractID.which_contract = Hedera_ContractID_evm_address_tag;
    tx.contractID.contract.evm_address.size = 19;
    assert_false(validate_and_reformat_contract_call(&tx));
}

static void test_contract_call_too_long_calldata(void **state) {
    (void)state;
    reset_ctx();
    Hedera_ContractCallTransactionBody tx = Hedera_ContractCallTransactionBody_init_zero;
    // selector + 3 words (too long)
    tx.functionParameters.size = 4 + 32 + 32 + 32;
    tx.functionParameters.bytes[0] = 0xA9;
    tx.functionParameters.bytes[1] = 0x05;
    tx.functionParameters.bytes[2] = 0x9C;
    tx.functionParameters.bytes[3] = 0xBB;
    assert_false(validate_and_reformat_contract_call(&tx));
}

static void test_handle_contract_call_body_throws_on_invalid(void **state) {
    (void)state;
    reset_ctx();
    // Prepare st_ctx with invalid calldata (wrong length)
    st_ctx.transaction.data.contractCall.functionParameters.size = 4 + 32; // too short
    st_ctx.transaction.data.contractCall.functionParameters.bytes[0] = 0xA9;
    st_ctx.transaction.data.contractCall.functionParameters.bytes[1] = 0x05;
    st_ctx.transaction.data.contractCall.functionParameters.bytes[2] = 0x9C;
    st_ctx.transaction.data.contractCall.functionParameters.bytes[3] = 0xBB;
    g_last_throw = 0;
    handle_contract_call_body();
    assert_int_equal(g_last_throw, EXCEPTION_MALFORMED_APDU);
}

static void test_contract_call_just_over_limit(void **state) {
    (void)state;
    reset_ctx();
    Hedera_ContractCallTransactionBody tx = Hedera_ContractCallTransactionBody_init_zero;
    tx.functionParameters.size = 1025;
    g_last_throw = 0;
    assert_false(validate_and_reformat_contract_call(&tx));
    handle_contract_call_body();
    assert_int_equal(g_last_throw, EXCEPTION_MALFORMED_APDU);
    tx.functionParameters.size = 4 + 32 + 32;
    tx.functionParameters.bytes[0] = 0xA9;
    tx.functionParameters.bytes[1] = 0x05;
    tx.functionParameters.bytes[2] = 0x9C;
    tx.functionParameters.bytes[3] = 0xBB;
    for (int i = 0; i < 32; i++) tx.functionParameters.bytes[4 + i] = 0x00;
    for (int i = 0; i < 32; i++) tx.functionParameters.bytes[4 + 32 + i] = 0x00;
    assert_false(validate_and_reformat_contract_call(&tx));
    handle_contract_call_body();
    assert_int_equal(g_last_throw, EXCEPTION_MALFORMED_APDU);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_contract_call_valid_evm_address),
        cmocka_unit_test(test_contract_call_invalid_params_length),
        cmocka_unit_test(test_contract_call_invalid_evm_address_size),
        cmocka_unit_test(test_contract_call_too_long_calldata),
        cmocka_unit_test(test_handle_contract_call_body_throws_on_invalid),
        cmocka_unit_test(test_contract_call_just_over_limit),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}


