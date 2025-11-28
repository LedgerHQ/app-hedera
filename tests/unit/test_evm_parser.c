#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "evm_parser.h"
#include "ui/app_globals.h"
#include <string.h>

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
    assert_true(strcmp(addr, "0x2222222222222222222222222222222222222222") == 0);

    char dec[MAX_UINT256_LENGTH + 1];
    assert_true(evm_word_to_amount(out.amount.bytes, dec));
    // Just sanity check: first digit should be '1' for (2^256-1) in decimal starts with '1'
    assert_int_equal(dec[0], '1');
}

// Helper to set a 64-bit value into a 32-byte big-endian uint256 buffer
static void set_u256_from_u64(uint8_t out[32], uint64_t value) {
    memset(out, 0, 32);
    // write as big-endian into the last 8 bytes
    for (int i = 0; i < 8; i++) {
        out[31 - i] = (uint8_t)(value & 0xFF);
        value >>= 8;
    }
}

static void test_amount_round_trims_zeros(void **state) {
    (void)state;
    char out[256] = {0};

    // 100000 with 5 decimals -> 1
    uint8_t amt1[32];
    set_u256_from_u64(amt1, 100000ULL);
    assert_true(evm_amount_to_string(amt1, 32, 5, NULL, out, sizeof(out)));
    assert_string_equal(out, "1");
    assert_true(strcmp(out, "1") == 0);

    // 123450000 with 4 decimals -> 12345
    uint8_t amt2[32];
    set_u256_from_u64(amt2, 123450000ULL);
    memset(out, 0, sizeof(out));
    assert_true(evm_amount_to_string(amt2, 32, 4, NULL, out, sizeof(out)));
    assert_string_equal(out, "12345");

    // 120000 with 5 decimals -> 1.2 (trailing fractional zeros trimmed)
    uint8_t amt3[32];
    set_u256_from_u64(amt3, 120000ULL);
    memset(out, 0, sizeof(out));
    assert_true(evm_amount_to_string(amt3, 32, 5, NULL, out, sizeof(out)));
    assert_string_equal(out, "1.2");
}

static void test_zero_across_decimals(void **state) {
    (void)state;
    uint8_t zero[32] = {0};
    char out[64] = {0};
    // decimals: 0, 6, 18, 77
    uint8_t test_decimals[] = {0, 6, 18, 77};
    for (size_t i = 0; i < sizeof(test_decimals); i++) {
        memset(out, 0, sizeof(out));
        assert_true(evm_amount_to_string(zero, 32, test_decimals[i], NULL, out, sizeof(out)));
        assert_string_equal(out, "0");
    }
}

static void test_smallest_with_large_decimals(void **state) {
    (void)state;
    uint8_t one[32] = {0};
    one[31] = 0x01; // value = 1

    char out[512] = {0};

    // decimals = 77 -> "0." + 76*"0" + "1"
    assert_true(evm_amount_to_string(one, 32, 77, NULL, out, sizeof(out)));
    char expected77[80 + 8] = {0};
    expected77[0] = '0'; expected77[1] = '.';
    memset(expected77 + 2, '0', 76);
    expected77[78] = '1'; expected77[79] = '\0';
    assert_true(strcmp(out, expected77) == 0);
    size_t len = strlen(out);
    assert_int_equal(out[0], '0');
    assert_int_equal(out[1], '.');
    // verify zeros count
    size_t zeros = 0;
    for (size_t i = 2; i + 1 < len; i++) if (out[i] == '0') zeros++;
    assert_int_equal(zeros, 76);
    assert_int_equal(out[len - 1], '1');

    // decimals = 100 -> "0." + 99*"0" + "1"
    memset(out, 0, sizeof(out));
    assert_true(evm_amount_to_string(one, 32, 100, NULL, out, sizeof(out)));
    char expected100[104 + 8] = {0};
    expected100[0] = '0'; expected100[1] = '.';
    memset(expected100 + 2, '0', 99);
    expected100[101] = '1'; expected100[102] = '\0';
    assert_true(strcmp(out, expected100) == 0);
    len = strlen(out);
    assert_int_equal(out[0], '0');
    assert_int_equal(out[1], '.');
    zeros = 0;
    for (size_t i = 2; i + 1 < len; i++) if (out[i] == '0') zeros++;
    assert_int_equal(zeros, 99);
    assert_int_equal(out[len - 1], '1');
}

static void test_max_with_various_decimals(void **state) {
    (void)state;
    uint8_t maxv[32];
    for (int i = 0; i < 32; i++) maxv[i] = 0xFF;

    char out[256] = {0};

    // decimals = 0 -> 78-digit integer, starts with '1'
    assert_true(evm_amount_to_string(maxv, 32, 0, NULL, out, sizeof(out)));
    assert_int_equal(out[0], '1');
    assert_int_equal(strlen(out), 78);

    // decimals = 78 -> "0." + 78 digits
    memset(out, 0, sizeof(out));
    assert_true(evm_amount_to_string(maxv, 32, 78, NULL, out, sizeof(out)));
    assert_int_equal(out[0], '0');
    assert_int_equal(out[1], '.');
    assert_int_equal(out[2], '1');
    assert_int_equal(strlen(out), 80);

    // decimals = 1 -> should contain one dot and not end with '.'
    memset(out, 0, sizeof(out));
    assert_true(evm_amount_to_string(maxv, 32, 1, NULL, out, sizeof(out)));
    size_t len = strlen(out);
    assert_true(strchr(out, '.') != NULL);
    assert_true(out[len - 1] != '.');
}

static void test_insufficient_buffer(void **state) {
    (void)state;
    uint8_t one[32] = {0};
    one[31] = 0x01;
    char small[8] = {0};
    // With large decimals the required buffer is much bigger than 8
    assert_false(evm_amount_to_string(one, 32, 60, NULL, small, sizeof(small)));
}

static void test_ticker_after_amount(void **state) {
    (void)state;
    // amount = 123456 with 4 decimals -> 12.3456 THEN ticker
    uint8_t amt[32];
    set_u256_from_u64(amt, 123456ULL);
    char out[128] = {0};
    assert_true(evm_amount_to_string(amt, 32, 4, "TOK", out, sizeof(out)));
    assert_string_equal(out, "12.3456 TOK");
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_parse_transfer_valid),
        cmocka_unit_test(test_parse_transfer_invalid_selector),
        cmocka_unit_test(test_format_hex_and_decimal),
        cmocka_unit_test(test_parse_transfer_invalid_length),
        cmocka_unit_test(test_parse_transfer_nonzero_padding_and_max_amount),
        cmocka_unit_test(test_amount_round_trims_zeros),
        cmocka_unit_test(test_zero_across_decimals),
        cmocka_unit_test(test_smallest_with_large_decimals),
        cmocka_unit_test(test_max_with_various_decimals),
        cmocka_unit_test(test_insufficient_buffer),
        cmocka_unit_test(test_ticker_after_amount),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}


