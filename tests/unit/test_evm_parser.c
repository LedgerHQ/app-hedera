#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "evm_parser.h"
#include "ui/app_globals.h"

static void test_parse_transfer_valid(void **state) {
    (void)state;
    uint8_t calldata[4 + 32 + 32] = {0};
    // selector a9059cbb
    calldata[0] = 0xA9; calldata[1] = 0x05; calldata[2] = 0x9C; calldata[3] = 0xBB;
    // address: last 20 bytes set to 0x11
    for (int i = 0; i < 20; i++) calldata[4 + 12 + i] = 0x11;
    // amount: set to 0x01...
    calldata[4 + 32] = 0x00; // high bytes zero
    calldata[4 + 32 + 31] = 0x01; // low byte = 1

    transfer_calldata_t out = {0};
    assert_true(parse_transfer_function(calldata, sizeof(calldata), &out));

    char addr[EVM_ADDRESS_STR_SIZE];
    assert_true(evm_addr_to_str(&out.to, addr, sizeof(addr)));
    assert_string_equal(addr, "0x1111111111111111111111111111111111111111");

    char amount[MAX_UINT256_LENGTH + 1];
    assert_true(evm_word_to_amount(out.amount.bytes, amount));
    assert_string_equal(amount, "1");
}

static void test_parse_transfer_invalid_length(void **state) {
    (void)state;
    uint8_t calldata[4 + 32 + 64] = {0};
    // wrong length
    calldata[0] = 0xA9; calldata[1] = 0x05; calldata[2] = 0x9C; calldata[3] = 0xBB;
    calldata[4 + 32] = 0x00; // high bytes zero
    calldata[4 + 32 + 31] = 0x01; // low byte = 1
    // extra 32 bytes
    calldata[4 + 32 + 32] = 0x00; // high bytes zero
    calldata[4 + 32 + 32 + 31] = 0x01; // low byte = 1
    transfer_calldata_t out = {0};
    assert_false(parse_transfer_function(calldata, sizeof(calldata), &out));
}

static void test_parse_transfer_invalid_selector(void **state) {
    (void)state;
    uint8_t calldata[4 + 32 + 32] = {0};
    // wrong selector
    calldata[0] = 0xDE; calldata[1] = 0xAD; calldata[2] = 0xBE; calldata[3] = 0xEF;
    transfer_calldata_t out = {0};
    assert_false(parse_transfer_function(calldata, sizeof(calldata), &out));
}

static void test_format_hex_and_decimal(void **state) {
    (void)state;
    uint8_t word[32] = {0};
    word[31] = 0xFF; // decimal 255
    char dec[MAX_UINT256_LENGTH + 1];
    assert_true(evm_word_to_amount(word, dec));
    assert_string_equal(dec, "255");

    char hex[EVM_WORD_STR_SIZE];
    assert_true(evm_word_to_str(word, hex));
    assert_string_equal(hex, "0x00000000000000000000000000000000000000000000000000000000000000ff");
}

static void test_parse_transfer_nonzero_padding_and_max_amount(void **state) {
    (void)state;
    uint8_t calldata[4 + 32 + 32] = {0};
    // selector a9059cbb
    calldata[0] = 0xA9; calldata[1] = 0x05; calldata[2] = 0x9C; calldata[3] = 0xBB;
    // address word: non-zero padding (should be ignored), address bytes = 0x22
    for (int i = 0; i < 12; i++) calldata[4 + i] = 0xAA;
    for (int i = 0; i < 20; i++) calldata[4 + 12 + i] = 0x22;
    // amount word: max value 0xFF..FF
    for (int i = 0; i < 32; i++) calldata[4 + 32 + i] = 0xFF;
    transfer_calldata_t out = {0};
    assert_true(parse_transfer_function(calldata, sizeof(calldata), &out));

    char addr[EVM_ADDRESS_STR_SIZE];
    assert_true(evm_addr_to_str(&out.to, addr, sizeof(addr)));
    assert_string_equal(addr, "0x2222222222222222222222222222222222222222");

    char dec[MAX_UINT256_LENGTH + 1];
    assert_true(evm_word_to_amount(out.amount.bytes, dec));
    // Just sanity check: first digit should be '1' for (2^256-1) in decimal starts with '1'
    assert_int_equal(dec[0], '1');
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_parse_transfer_valid),
        cmocka_unit_test(test_parse_transfer_invalid_selector),
        cmocka_unit_test(test_format_hex_and_decimal),
        cmocka_unit_test(test_parse_transfer_invalid_length),
        cmocka_unit_test(test_parse_transfer_nonzero_padding_and_max_amount),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}


