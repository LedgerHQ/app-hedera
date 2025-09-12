#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <string.h>
#include <cmocka.h>

#define PB_SYSTEM_HEADER "nanopb_system.h"
#include <pb_decode.h>
#include <pb_encode.h>

#include "contract_call.pb.h"
#include "basic_types.pb.h"

#include "sign_contract_call.h"
#include "ui/app_globals.h"

sign_tx_context_t st_ctx; // define global for validator side-effects

static void build_erc20_transfer_calldata(uint8_t *out, size_t out_len) {
    assert_non_null(out);
    assert_int_equal(out_len, 4 + 32 + 32);
    memset(out, 0, out_len);
    // selector a9059cbb
    out[0] = 0xA9; out[1] = 0x05; out[2] = 0x9C; out[3] = 0xBB;
    // address word: 12 bytes padding + 20 bytes address (0x33...)
    for (int i = 0; i < 20; i++) out[4 + 12 + i] = 0x33;
    // amount word: = 1
    out[4 + 32 + 31] = 0x01;
}

static void test_pb_decode_erc20_and_format(void **state) {
    (void)state;
    uint8_t buffer[1024];
    Hedera_ContractCallTransactionBody tx = Hedera_ContractCallTransactionBody_init_zero;

    // Contract ID as EVM address 0x44...44
    tx.has_contractID = true;
    tx.contractID.which_contract = Hedera_ContractID_evm_address_tag;
    tx.contractID.contract.evm_address.size = 20;
    for (int i = 0; i < 20; i++) tx.contractID.contract.evm_address.bytes[i] = 0x44;

    tx.gas = 123;
    tx.amount = 456;

    // ERC20 calldata
    tx.functionParameters.size = 4 + 32 + 32;
    build_erc20_transfer_calldata(tx.functionParameters.bytes, tx.functionParameters.size);

    // Encode
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    assert_true(pb_encode(&ostream, Hedera_ContractCallTransactionBody_fields, &tx));

    // Decode
    Hedera_ContractCallTransactionBody decoded = Hedera_ContractCallTransactionBody_init_zero;
    pb_istream_t istream = pb_istream_from_buffer(buffer, ostream.bytes_written);
    assert_true(pb_decode(&istream, Hedera_ContractCallTransactionBody_fields, &decoded));

    // Validate and format
    memset(&st_ctx, 0, sizeof(st_ctx));
    assert_true(validate_and_reformat_contract_call(&decoded));

    // Check UI side-effects
    assert_string_equal(st_ctx.senders, "0x4444444444444444444444444444444444444444");
    assert_string_equal(st_ctx.recipients, "0x3333333333333333333333333333333333333333");
    assert_string_not_equal(st_ctx.amount, "");
}

static void test_pb_decode_erc20_too_short_params(void **state) {
    (void)state;
    uint8_t buffer[256];
    Hedera_ContractCallTransactionBody tx = Hedera_ContractCallTransactionBody_init_zero;

    tx.functionParameters.size = 4 + 32; // too short
    memset(tx.functionParameters.bytes, 0, tx.functionParameters.size);
    tx.functionParameters.bytes[0] = 0xA9; tx.functionParameters.bytes[1] = 0x05;
    tx.functionParameters.bytes[2] = 0x9C; tx.functionParameters.bytes[3] = 0xBB;

    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    assert_true(pb_encode(&ostream, Hedera_ContractCallTransactionBody_fields, &tx));

    Hedera_ContractCallTransactionBody decoded = Hedera_ContractCallTransactionBody_init_zero;
    pb_istream_t istream = pb_istream_from_buffer(buffer, ostream.bytes_written);
    assert_true(pb_decode(&istream, Hedera_ContractCallTransactionBody_fields, &decoded));

    memset(&st_ctx, 0, sizeof(st_ctx));
    assert_false(validate_and_reformat_contract_call(&decoded));
}

static void test_pb_decode_erc20_too_long_params(void **state) {
    (void)state;
    uint8_t buffer[256];
    Hedera_ContractCallTransactionBody tx = Hedera_ContractCallTransactionBody_init_zero;

    tx.functionParameters.size = 4 + 32 + 32 + 1; // too long
    memset(tx.functionParameters.bytes, 0, tx.functionParameters.size);
    tx.functionParameters.bytes[0] = 0xA9; tx.functionParameters.bytes[1] = 0x05;
    tx.functionParameters.bytes[2] = 0x9C; tx.functionParameters.bytes[3] = 0xBB;

    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    assert_true(pb_encode(&ostream, Hedera_ContractCallTransactionBody_fields, &tx));

    Hedera_ContractCallTransactionBody decoded = Hedera_ContractCallTransactionBody_init_zero;
    pb_istream_t istream = pb_istream_from_buffer(buffer, ostream.bytes_written);
    assert_true(pb_decode(&istream, Hedera_ContractCallTransactionBody_fields, &decoded));

    memset(&st_ctx, 0, sizeof(st_ctx));
    assert_false(validate_and_reformat_contract_call(&decoded));
}

static void test_pb_decode_malformed_length_field(void **state) {
    (void)state;
    // Field #4 (functionParameters), wire type 2 (length-delimited), with huge length and no payload
    uint8_t buf[] = { (Hedera_ContractCallTransactionBody_functionParameters_tag << 3) | 2, 0xFF, 0x7F };
    Hedera_ContractCallTransactionBody decoded = Hedera_ContractCallTransactionBody_init_zero;
    pb_istream_t istream = pb_istream_from_buffer(buf, sizeof(buf));
    assert_false(pb_decode(&istream, Hedera_ContractCallTransactionBody_fields, &decoded));

}

static void test_pb_decode_evm_address_wrong_size(void **state) {
    (void)state;
    uint8_t buffer[256];
    Hedera_ContractCallTransactionBody tx = Hedera_ContractCallTransactionBody_init_zero;

    // Wrong EVM address size
    tx.has_contractID = true;
    tx.contractID.which_contract = Hedera_ContractID_evm_address_tag;
    tx.contractID.contract.evm_address.size = 19; // invalid

    // Valid selector and length for transfer params
    tx.functionParameters.size = 4 + 32 + 32;
    build_erc20_transfer_calldata(tx.functionParameters.bytes, tx.functionParameters.size);

    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    assert_true(pb_encode(&ostream, Hedera_ContractCallTransactionBody_fields, &tx));

    Hedera_ContractCallTransactionBody decoded = Hedera_ContractCallTransactionBody_init_zero;
    pb_istream_t istream = pb_istream_from_buffer(buffer, ostream.bytes_written);
    assert_true(pb_decode(&istream, Hedera_ContractCallTransactionBody_fields, &decoded));

    memset(&st_ctx, 0, sizeof(st_ctx));
    assert_false(validate_and_reformat_contract_call(&decoded));
}

static void test_pb_decode_evm_address_too_long_size(void **state) {
    (void)state;    
    uint8_t buffer[1024];
    Hedera_ContractCallTransactionBody tx = Hedera_ContractCallTransactionBody_init_zero;

    // Wrong EVM address size
    tx.has_contractID = true;
    tx.contractID.which_contract = Hedera_ContractID_evm_address_tag;
    tx.contractID.contract.evm_address.size = 20; // invalid

    // Valid selector and length for transfer params
    tx.functionParameters.size = 4 + 32 + 32;
    build_erc20_transfer_calldata(tx.functionParameters.bytes, tx.functionParameters.size);

    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    assert_true(pb_encode(&ostream, Hedera_ContractCallTransactionBody_fields, &tx));

    Hedera_ContractCallTransactionBody decoded = Hedera_ContractCallTransactionBody_init_zero;
    pb_istream_t istream = pb_istream_from_buffer(buffer, ostream.bytes_written);
    assert_true(pb_decode(&istream, Hedera_ContractCallTransactionBody_fields, &decoded));

    // Modify evm_address length inside encoded ContractID submessage to 21
    // Find outer field 1 (contractID: tag 0x0A)
    size_t i = 0;
    size_t enc_len = ostream.bytes_written;
    while (i + 1 < enc_len && buffer[i] != 0x0A) i++;
    assert_true(i + 1 < enc_len);
    // contractID submessage length (single-byte varint expected here)
    uint8_t cid_len = buffer[i + 1];
    size_t cid_start = i + 2;
    assert_true(cid_start + cid_len <= enc_len);
    // Inside contractID, find field 4 tag (evm_address: 0x22) and bump its length
    size_t j = cid_start;
    bool patched = false;
    while (j + 1 < cid_start + cid_len) {
        if (buffer[j] == 0x22) {
            // Next byte is length; change 0x14 (20) to 0x15 (21)
            buffer[j + 1] = 0x15;
            patched = true;
            break;
        }
        j++;
    }
    assert_true(patched);

    Hedera_ContractCallTransactionBody decoded1 = Hedera_ContractCallTransactionBody_init_zero;
    istream = pb_istream_from_buffer(buffer, ostream.bytes_written);
    assert_false(pb_decode(&istream, Hedera_ContractCallTransactionBody_fields, &decoded1));
}

static void test_pb_decode_negative_gas_and_amount(void **state) {
    (void)state;
    uint8_t buffer[1024];
    Hedera_ContractCallTransactionBody tx = Hedera_ContractCallTransactionBody_init_zero;

    tx.has_contractID = true;
    tx.contractID.which_contract = Hedera_ContractID_contractNum_tag;
    tx.contractID.shardNum = 0; tx.contractID.realmNum = 0; tx.contractID.contract.contractNum = 1;

    tx.gas = -1;
    tx.amount = -1;
    tx.functionParameters.size = 4 + 32 + 32;
    build_erc20_transfer_calldata(tx.functionParameters.bytes, tx.functionParameters.size);

    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    assert_true(pb_encode(&ostream, Hedera_ContractCallTransactionBody_fields, &tx));

    Hedera_ContractCallTransactionBody decoded = Hedera_ContractCallTransactionBody_init_zero;
    pb_istream_t istream = pb_istream_from_buffer(buffer, ostream.bytes_written);
    assert_true(pb_decode(&istream, Hedera_ContractCallTransactionBody_fields, &decoded));

    memset(&st_ctx, 0, sizeof(st_ctx));
    assert_false(validate_and_reformat_contract_call(&decoded));
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_pb_decode_erc20_and_format),
        cmocka_unit_test(test_pb_decode_erc20_too_short_params),
        cmocka_unit_test(test_pb_decode_erc20_too_long_params),
        cmocka_unit_test(test_pb_decode_malformed_length_field),
        cmocka_unit_test(test_pb_decode_evm_address_wrong_size),
        cmocka_unit_test(test_pb_decode_evm_address_too_long_size),
        cmocka_unit_test(test_pb_decode_negative_gas_and_amount),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}


