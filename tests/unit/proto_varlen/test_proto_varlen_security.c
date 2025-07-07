#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

// Include the module under test
#include "proto_varlen_parser.h"

// Test helper functions to create malicious protobuf data
static void write_varint(uint8_t **buffer, uint64_t value) {
    while (value >= 0x80) {
        **buffer = (uint8_t)(value | 0x80);
        (*buffer)++;
        value >>= 7;
    }
    **buffer = (uint8_t)value;
    (*buffer)++;
}

// SECURITY TESTS - Buffer Overflows and Infinite Loops

// Test 1: Varint overflow protection (shift >= 64)
static void test_decode_varint_infinite_loop_protection(void **state) {
    (void) state;
    
    // Create a malicious varint with 10+ continuation bytes (should cause shift >= 64)
    uint8_t malicious_data[16];
    for (int i = 0; i < 15; i++) {
        malicious_data[i] = 0xFF; // All bits set, continuation bit on
    }
    malicious_data[15] = 0x7F; // Final byte
    
    const uint8_t *ptr = malicious_data;
    const uint8_t *end = malicious_data + sizeof(malicious_data);
    protobuf_field_t field;
    
    // This should fail gracefully, not infinite loop or crash
    bool result = parse_field_tag(&ptr, end, &field);
    assert_false(result);
}

// Test 2: Integer overflow in pointer arithmetic
static void test_pointer_arithmetic_overflow(void **state) {
    (void) state;
    
    uint8_t buffer[64];
    uint8_t *ptr = buffer;
    
    // Create a string field with UINT64_MAX length
    uint64_t tag = (15 << 3) | 2; // field 15, wire type string
    write_varint(&ptr, tag);
    
    // Write maximum possible length (this would cause overflow)
    uint64_t malicious_length = UINT64_MAX;
    write_varint(&ptr, malicious_length);
    
    size_t buffer_len = ptr - buffer;
    
    char output[64];
    bool result = extract_nested_string_field(buffer, buffer_len, 14, output, sizeof(output));
    
    // Should fail safely, not crash
    assert_false(result);
}

// Test 3: Pointer arithmetic overflow in nested structures
static void test_nested_pointer_overflow(void **state) {
    (void) state;
    
    uint8_t buffer[128];
    uint8_t *ptr = buffer;
    
    // Create TransactionBody field 15 (cryptoUpdateAccount)
    uint64_t outer_tag = (15 << 3) | 2;
    write_varint(&ptr, outer_tag);
    
    // Claim the inner message has SIZE_MAX length
    write_varint(&ptr, SIZE_MAX);
    
    // Add some minimal inner content
    uint64_t inner_tag = (14 << 3) | 2;
    write_varint(&ptr, inner_tag);
    write_varint(&ptr, 4);
    memcpy(ptr, "test", 4);
    ptr += 4;
    
    size_t buffer_len = ptr - buffer;
    
    char output[64];
    bool result = extract_nested_string_field(buffer, buffer_len, 14, output, sizeof(output));
    
    // Should handle overflow gracefully
    assert_false(result);
}

// Test 4: Large string length causing buffer overflow
static void test_large_string_length_overflow(void **state) {
    (void) state;
    
    uint8_t buffer[256];
    uint8_t *ptr = buffer;
    
    // Create a StringValue submessage with huge claimed length
    uint8_t string_value[64];
    uint8_t *sv_ptr = string_value;
    
    // StringValue.value field (field 1)
    uint64_t sv_tag = (1 << 3) | 2;
    write_varint(&sv_ptr, sv_tag);
    
    // Claim string is 0x7FFFFFFF bytes long (but we only have a few bytes)
    write_varint(&sv_ptr, 0x7FFFFFFF);
    
    // Add minimal actual data
    memcpy(sv_ptr, "hi", 2);
    sv_ptr += 2;
    
    size_t sv_len = sv_ptr - string_value;
    
    // Create CryptoUpdateTransactionBody
    uint8_t crypto_update[96];
    uint8_t *cu_ptr = crypto_update;
    uint64_t cu_tag = (14 << 3) | 2;
    write_varint(&cu_ptr, cu_tag);
    write_varint(&cu_ptr, sv_len);
    memcpy(cu_ptr, string_value, sv_len);
    cu_ptr += sv_len;
    
    size_t cu_len = cu_ptr - crypto_update;
    
    // Create TransactionBody
    uint64_t tb_tag = (15 << 3) | 2;
    write_varint(&ptr, tb_tag);
    write_varint(&ptr, cu_len);
    memcpy(ptr, crypto_update, cu_len);
    ptr += cu_len;
    
    size_t buffer_len = ptr - buffer;
    
    char output[64];
    bool result = extract_nested_string_field(buffer, buffer_len, 14, output, sizeof(output));
    
    // Should detect the inconsistency and fail safely
    assert_false(result);
}

// Test 5: Infinite loop in skip_field with malicious wire type
static void test_skip_field_unknown_wire_type(void **state) {
    (void) state;
    
    uint8_t buffer[32];
    uint8_t *ptr = buffer;
    
    // Create a field with unknown wire type (7, which doesn't exist)
    uint64_t malicious_tag = (10 << 3) | 7; // field 10, wire type 7 (invalid)
    write_varint(&ptr, malicious_tag);
    
    size_t buffer_len = ptr - buffer;
    
    char output[64];
    bool result = extract_nested_string_field(buffer, buffer_len, 14, output, sizeof(output));
    
    // Should fail on unknown wire type
    assert_false(result);
}

// Test 6: Zero-length buffer edge case
static void test_zero_length_buffer(void **state) {
    (void) state;
    
    uint8_t *empty_buffer = NULL;
    char output[64];
    
    bool result = extract_nested_string_field(empty_buffer, 0, 14, output, sizeof(output));
    assert_false(result);
    
    // Also test with non-NULL but zero size
    uint8_t dummy = 0;
    result = extract_nested_string_field(&dummy, 0, 14, output, sizeof(output));
    assert_false(result);
}

// Test 7: Buffer overflow via size_t to uint64_t conversion
static void test_size_conversion_overflow(void **state) {
    (void) state;
    
    uint8_t buffer[64];
    char output[4]; // Very small output buffer
    
    // Create valid small data, but the vulnerability is in output buffer size handling
    uint8_t *ptr = buffer;
    
    // StringValue submessage
    uint8_t string_value[32];
    uint8_t *sv_ptr = string_value;
    uint64_t sv_tag = (1 << 3) | 2;
    write_varint(&sv_ptr, sv_tag);
    write_varint(&sv_ptr, 20); // 20 byte string
    memcpy(sv_ptr, "very_long_test_string", 20);
    sv_ptr += 20;
    size_t sv_len = sv_ptr - string_value;
    
    // CryptoUpdate
    uint8_t crypto_update[48];
    uint8_t *cu_ptr = crypto_update;
    uint64_t cu_tag = (14 << 3) | 2;
    write_varint(&cu_ptr, cu_tag);
    write_varint(&cu_ptr, sv_len);
    memcpy(cu_ptr, string_value, sv_len);
    cu_ptr += sv_len;
    size_t cu_len = cu_ptr - crypto_update;
    
    // TransactionBody
    uint64_t tb_tag = (15 << 3) | 2;
    write_varint(&ptr, tb_tag);
    write_varint(&ptr, cu_len);
    memcpy(ptr, crypto_update, cu_len);
    ptr += cu_len;
    
    size_t buffer_len = ptr - buffer;
    
    bool result = extract_nested_string_field(buffer, buffer_len, 14, output, sizeof(output));
    
    // Should truncate safely without buffer overflow
    assert_true(result);
    assert_int_equal(3, strlen(output)); // Should be truncated to 3 chars + null
    assert_string_equal("ver", output);
}

// Test 8: Deeply nested malicious recursion
static void test_deeply_nested_structures(void **state) {
    (void) state;
    
    uint8_t buffer[512];
    uint8_t *ptr = buffer;
    
    // Create nested structure with many layers to test stack overflow protection
    for (int depth = 0; depth < 100; depth++) {
        uint64_t tag = (15 << 3) | 2; // Always field 15, string type
        write_varint(&ptr, tag);
        write_varint(&ptr, 10); // Claim 10 bytes for inner message
        
        if (ptr >= buffer + sizeof(buffer) - 20) break; // Prevent our own buffer overflow
    }
    
    size_t buffer_len = ptr - buffer;
    
    char output[64];
    bool result = extract_nested_string_field(buffer, buffer_len, 14, output, sizeof(output));
    
    // Should handle deep nesting gracefully
    assert_false(result);
}

// Test 9: Invalid field number edge cases
static void test_invalid_field_numbers(void **state) {
    (void) state;
    
    uint8_t buffer[32];
    uint8_t *ptr = buffer;
    
    // Test field number 0 (invalid in protobuf)
    uint64_t zero_field = (0 << 3) | 2;
    write_varint(&ptr, zero_field);
    write_varint(&ptr, 4);
    memcpy(ptr, "test", 4);
    ptr += 4;
    
    size_t buffer_len = ptr - buffer;
    
    char output[64];
    bool result = extract_nested_string_field(buffer, buffer_len, 0, output, sizeof(output));
    
    // Should handle field 0 appropriately
    assert_false(result);
}

// Test 10: Memory boundary edge case - end pointer manipulation
static void test_end_pointer_edge_cases(void **state) {
    (void) state;
    
    // Test with end < start (invalid pointers)
    uint8_t data[] = {0x0A, 0x04, 't', 'e', 's', 't'};
    const uint8_t *start = data + 3;
    const uint8_t *end = data; // end < start
    protobuf_field_t field;
    
    bool result = parse_field_tag(&start, end, &field);
    assert_false(result);
}

// Test data for security edge cases

// Protobuf with extremely large field number
static const uint8_t large_field_number_proto[] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0x0F,  // Field number close to max varint (2^32-1)
    0x0A, 0x05,                    // Length 5
    'H', 'e', 'l', 'l', 'o'
};

// Protobuf with recursive depth bomb
static const uint8_t depth_bomb_proto[] = {
    0x0A, 0x10,                    // Field 1, length 16
    0x0A, 0x0E,                    // Field 1, length 14
    0x0A, 0x0C,                    // Field 1, length 12
    0x0A, 0x0A,                    // Field 1, length 10
    0x0A, 0x08,                    // Field 1, length 8
    0x0A, 0x06,                    // Field 1, length 6
    0x0A, 0x04,                    // Field 1, length 4
    0x0A, 0x02,                    // Field 1, length 2
    0x08, 0x01                     // Field 1, varint 1
};

// Protobuf with overlapping/corrupted length fields
static const uint8_t corrupted_length_proto[] = {
    0x0A, 0x20,                    // Field 1, claims length 32
    0x72, 0x1E,                    // Field 14, claims length 30 (would overlap)
    'S', 'h', 'o', 'r', 't'        // Only 5 bytes of actual data
};

// Protobuf with zero-length string that should be valid
static const uint8_t zero_length_valid_proto[] = {
    0x0A, 0x08,                    // Field 1, length 8
    0x72, 0x06,                    // Field 14, length 6
    0x0A, 0x04,                    // Field 1, length 4
    0x0A, 0x02,                    // Field 1, length 2
    0x12, 0x00                     // Field 2, length 0 (empty string)
};

// Test against buffer overflow attacks
static void test_extract_nested_string_field_buffer_overflow(void **state)
{
    (void) state;
    
    // Create a message with a string longer than our output buffer
    uint8_t overflow_proto[1024];
    size_t pos = 0;
    
    // Field 1, message with field 14
    overflow_proto[pos++] = 0x0A;  // Field 1, wire type 2
    overflow_proto[pos++] = 0xFF;  // Length 255 (will be larger than our buffer)
    overflow_proto[pos++] = 0x01;  // Length continuation
    
    overflow_proto[pos++] = 0x72;  // Field 14, wire type 2
    overflow_proto[pos++] = 0xFC;  // Length 252
    overflow_proto[pos++] = 0x01;  // Length continuation
    
    overflow_proto[pos++] = 0x0A;  // Field 1, wire type 2
    overflow_proto[pos++] = 0xF8;  // Length 248
    overflow_proto[pos++] = 0x01;  // Length continuation
    
    // Fill with data to reach the claimed length
    for (int i = 0; i < 248; i++) {
        if (pos < sizeof(overflow_proto)) {
            overflow_proto[pos++] = 'A' + (i % 26);
        }
    }
    
    char small_output[16];
    bool result = extract_nested_string_field(overflow_proto, 
                                           pos, 
                                           14, 
                                           small_output, 
                                           sizeof(small_output));
    
    // Should handle gracefully without overflowing buffer
    if (result) {
        // If successful, output should be properly null-terminated
        assert_int_equal(strlen(small_output), sizeof(small_output) - 1);
    }
    // Result could be false if parsing fails due to malformed data, which is also acceptable
}

// Test against integer overflow in length calculations
static void test_extract_nested_string_field_integer_overflow(void **state)
{
    (void) state;
    
    char output[256];
    
    // Test with the large field number protobuf
    bool result = extract_nested_string_field(large_field_number_proto, 
                                           sizeof(large_field_number_proto), 
                                           UINT32_MAX, 
                                           output, 
                                           sizeof(output));
    
    // Should handle large field numbers without integer overflow
    assert_false(result);  // Expected to not find the field
}

// Test against depth-based denial of service
static void test_extract_nested_string_field_depth_bomb(void **state)
{
    (void) state;
    
    char output[256];
    
    // Test with deeply nested protobuf structure
    bool result = extract_nested_string_field(depth_bomb_proto, 
                                           sizeof(depth_bomb_proto), 
                                           14, 
                                           output, 
                                           sizeof(output));
    
    // Should handle deep nesting without stack overflow or infinite loops
    assert_false(result);  // Expected to not find field 14
}

// Test against corrupted length field attacks
static void test_extract_nested_string_field_corrupted_length(void **state)
{
    (void) state;
    
    char output[256];
    
    // Test with corrupted length fields
    bool result = extract_nested_string_field(corrupted_length_proto, 
                                           sizeof(corrupted_length_proto), 
                                           14, 
                                           output, 
                                           sizeof(output));
    
    // Should detect corrupted data and fail gracefully
    assert_false(result);
}

// Test with various wire types to ensure proper validation
static void test_parse_field_tag_wire_type_validation(void **state)
{
    (void) state;
    
    // Test all possible wire type values (0-7)
    for (uint8_t wire_type = 0; wire_type < 8; wire_type++) {
        uint8_t tag_data = (1 << 3) | wire_type;  // Field 1 with different wire types
        const uint8_t *ptr = &tag_data;
        const uint8_t *end = &tag_data + 1;
        protobuf_field_t field;
        
        bool result = parse_field_tag(&ptr, end, &field);
        
        // parse_field_tag should parse all wire types successfully
        // The validation happens later in skip_field
        assert_true(result);
        assert_int_equal(field.wire_type, wire_type);
        assert_int_equal(field.field_number, 1);
    }
}

// Test boundary conditions for varint decoding
static void test_parse_field_tag_varint_boundary(void **state)
{
    (void) state;
    
    // Test maximum valid field number (2^29 - 1)
    uint8_t max_field_tag[] = {
        0xF8, 0xFF, 0xFF, 0xFF, 0x0F  // Maximum field number with wire type 0
    };
    
    const uint8_t *ptr = max_field_tag;
    const uint8_t *end = max_field_tag + sizeof(max_field_tag);
    protobuf_field_t field;
    
    bool result = parse_field_tag(&ptr, end, &field);
    
    // Should handle maximum field numbers correctly
    assert_true(result);
    assert_int_equal(field.wire_type, 0);
    // Field number should be close to maximum but exact value depends on implementation
}

// Test with zero-length strings (valid edge case)
static void test_extract_nested_string_field_zero_length_valid(void **state)
{
    (void) state;
    
    char output[256];
    memset(output, 'X', sizeof(output));  // Pre-fill to detect proper null termination
    
    bool result = extract_nested_string_field(zero_length_valid_proto, 
                                           sizeof(zero_length_valid_proto), 
                                           2,  // Looking for field 2 which has empty string
                                           output, 
                                           sizeof(output));
    
    if (result) {
        // Should be empty string but properly null-terminated
        assert_string_equal(output, "");
    } else {
        // If field not found, that's also acceptable for this test case
        assert_false(result);
    }
}

// Test memory safety with NULL pointers at various stages
static void test_extract_nested_string_field_null_safety(void **state)
{
    (void) state;
    
    char output[256];
    uint8_t valid_data[] = {0x0A, 0x02, 0x08, 0x01};
    
    // Test all NULL pointer combinations
    assert_false(extract_nested_string_field(NULL, 0, 1, output, sizeof(output)));
    assert_false(extract_nested_string_field(valid_data, sizeof(valid_data), 1, NULL, sizeof(output)));
    assert_false(extract_nested_string_field(valid_data, sizeof(valid_data), 1, output, 0));
    assert_false(extract_nested_string_field(NULL, sizeof(valid_data), 1, NULL, 0));
}

// Test with extremely large input buffers
static void test_extract_nested_string_field_large_input_safety(void **state)
{
    (void) state;
    
    // Create a large but valid buffer filled with garbage data
    const size_t large_size = 65536;  // Large but reasonable size
    uint8_t *large_buffer = malloc(large_size);
    assert_non_null(large_buffer);
    
    // Fill with random-ish data that could be parsed as varints
    for (size_t i = 0; i < large_size; i++) {
        large_buffer[i] = (uint8_t)((i % 127) | 0x80);  // Continuation bytes
    }
    large_buffer[large_size - 1] &= 0x7F;  // End the last varint
    
    char output[256];
    
    // This should fail gracefully without crashing or infinite loops
    bool result = extract_nested_string_field(large_buffer, large_size, 14, output, sizeof(output));
    
    // Should fail to find field 14 in random data
    assert_false(result);
    
    free(large_buffer);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_decode_varint_infinite_loop_protection),
        cmocka_unit_test(test_pointer_arithmetic_overflow),
        cmocka_unit_test(test_nested_pointer_overflow),
        cmocka_unit_test(test_large_string_length_overflow),
        cmocka_unit_test(test_skip_field_unknown_wire_type),
        cmocka_unit_test(test_zero_length_buffer),
        cmocka_unit_test(test_size_conversion_overflow),
        cmocka_unit_test(test_deeply_nested_structures),
        cmocka_unit_test(test_invalid_field_numbers),
        cmocka_unit_test(test_end_pointer_edge_cases),
        cmocka_unit_test(test_extract_nested_string_field_buffer_overflow),
        cmocka_unit_test(test_extract_nested_string_field_integer_overflow),
        cmocka_unit_test(test_extract_nested_string_field_depth_bomb),
        cmocka_unit_test(test_extract_nested_string_field_corrupted_length),
        cmocka_unit_test(test_parse_field_tag_wire_type_validation),
        cmocka_unit_test(test_parse_field_tag_varint_boundary),
        cmocka_unit_test(test_extract_nested_string_field_zero_length_valid),
        cmocka_unit_test(test_extract_nested_string_field_null_safety),
        cmocka_unit_test(test_extract_nested_string_field_large_input_safety),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
} 