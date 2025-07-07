#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <string.h>
#include <cmocka.h>

// Include the module under test
#include "proto_varlen_parser.h"

// Test helper functions to create protobuf data
static void write_varint(uint8_t **buffer, uint64_t value) {
    while (value >= 0x80) {
        **buffer = (uint8_t)(value | 0x80);
        (*buffer)++;
        value >>= 7;
    }
    **buffer = (uint8_t)value;
    (*buffer)++;
}

static void write_string_field(uint8_t **buffer, uint32_t field_number, const char *str) {
    // Write field tag (field_number << 3 | wire_type)
    write_varint(buffer, (field_number << 3) | 2);
    
    // Write string length
    size_t len = strlen(str);
    write_varint(buffer, len);
    
    // Write string data
    memcpy(*buffer, str, len);
    *buffer += len;
}

static void write_submessage_field(uint8_t **buffer, uint32_t field_number, 
                                   const uint8_t *submessage, size_t submessage_len) {
    // Write field tag
    write_varint(buffer, (field_number << 3) | 2);
    
    // Write submessage length
    write_varint(buffer, submessage_len);
    
    // Write submessage data
    memcpy(*buffer, submessage, submessage_len);
    *buffer += submessage_len;
}

// Test data: valid nested protobuf structure
// TransactionBody (field 15) -> CryptoUpdateTransactionBody (field 14) -> StringValue (field 1) -> "Test Value"
static const uint8_t valid_nested_proto[] = {
    0x7A, 0x10,                    // Field 15 (cryptoUpdateAccount), length 16
    0x72, 0x0E,                    // Field 14 (memo), length 14  
    0x0A, 0x0C,                    // Field 1 (value), length 12
    'T', 'e', 's', 't', ' ', 'V', 'a', 'l', 'u', 'e', '\0', '\0'  // "Test Value" + padding
};

// Test data: malformed protobuf
static const uint8_t malformed_proto[] = {
    0x7A, 0xFF,                    // Field 15, invalid length
    'T', 'e', 's', 't'             // Truncated data
};

// Test data: protobuf with deeply nested structure
static const uint8_t deeply_nested_proto[] = {
    0x7A, 0x1E,                    // Field 15, length 30
    0x72, 0x1C,                    // Field 14, length 28
    0x0A, 0x1A,                    // Field 1, length 26
    'D', 'e', 'e', 'p', 'l', 'y', ' ', 'N', 'e', 's', 't', 'e', 'd', ' ', 'V', 'a', 'l', 'u', 'e', '!', '\0', '\0', '\0', '\0', '\0', '\0'
};

// Test cases for parse_field_tag

static void test_parse_field_tag_valid(void **state) {
    (void) state; // unused
    
    const uint8_t tag_data[] = {0x72};  // Field 14, wire type 2
    const uint8_t *ptr = tag_data;
    const uint8_t *end = tag_data + sizeof(tag_data);
    protobuf_field_t field;
    
    bool result = parse_field_tag(&ptr, end, &field);
    
    assert_true(result);
    assert_int_equal(field.field_number, 14);
    assert_int_equal(field.wire_type, 2);
}

static void test_parse_field_tag_invalid_wire_type(void **state) {
    (void) state;
    const uint8_t tag_data[] = {0x77};  // Field 14, wire type 7 (invalid)
    const uint8_t *ptr = tag_data;
    const uint8_t *end = tag_data + sizeof(tag_data);
    protobuf_field_t field;
    
    bool result = parse_field_tag(&ptr, end, &field);
    
    // Parser should parse the tag successfully even with unknown wire type
    assert_true(result);
    assert_int_equal(field.field_number, 14);
    assert_int_equal(field.wire_type, 7);
}

static void test_parse_field_tag_truncated(void **state) {
    (void) state;
    const uint8_t *ptr = NULL;
    const uint8_t *end = NULL;
    protobuf_field_t field;
    
    bool result = parse_field_tag(&ptr, end, &field);
    
    assert_false(result);
}

// Test cases for extract_nested_string_field

static void test_extract_nested_string_field_valid(void **state) {
    (void) state;
    char output[256];
    
    // Test extracting field 14 from valid nested structure
    bool result = extract_nested_string_field(valid_nested_proto, 
                                           sizeof(valid_nested_proto), 
                                           14, 
                                           output, 
                                           sizeof(output));
    
    assert_true(result);
    assert_string_equal(output, "Test Value");
}

static void test_extract_nested_string_field_not_found(void **state) {
    (void) state;
    char output[256] = {0};
    
    // Test extracting non-existent field
    bool result = extract_nested_string_field(valid_nested_proto, 
                                           sizeof(valid_nested_proto), 
                                           99, 
                                           output, 
                                           sizeof(output));
    
    assert_false(result);
    assert_string_equal(output, "");
}

static void test_extract_nested_string_field_buffer_too_small(void **state) {
    (void) state;
    char small_output[5];
    
    // Test with buffer too small for result
    bool result = extract_nested_string_field(valid_nested_proto, 
                                           sizeof(valid_nested_proto), 
                                           14, 
                                           small_output, 
                                           sizeof(small_output));
    
    assert_true(result);
    assert_int_equal(strlen(small_output), 4);  // Should be truncated to fit
}

static void test_extract_nested_string_field_malformed_data(void **state) {
    (void) state;
    char output[256];
    
    // Test with malformed protobuf data
    bool result = extract_nested_string_field(malformed_proto, 
                                           sizeof(malformed_proto), 
                                           14, 
                                           output, 
                                           sizeof(output));
    
    assert_false(result);
}

static void test_extract_nested_string_field_empty_input(void **state) {
    (void) state;
    char output[256];
    
    // Test with empty input
    bool result = extract_nested_string_field(NULL, 0, 14, output, sizeof(output));
    assert_false(result);
    
    // Test with zero length
    result = extract_nested_string_field(valid_nested_proto, 0, 14, output, sizeof(output));
    assert_false(result);
}

static void test_extract_nested_string_field_null_output(void **state) {
    (void) state;
    
    // Test with NULL output buffer
    bool result = extract_nested_string_field(valid_nested_proto, 
                                           sizeof(valid_nested_proto), 
                                           14, 
                                           NULL, 
                                           256);
    
    assert_false(result);
}

static void test_extract_nested_string_field_deeply_nested(void **state) {
    (void) state;
    char output[256];
    
    // Test extracting from deeply nested structure
    bool result = extract_nested_string_field(deeply_nested_proto, 
                                           sizeof(deeply_nested_proto), 
                                           14, 
                                           output, 
                                           sizeof(output));
    
    assert_true(result);
    assert_string_equal(output, "Deeply Nested Value!");
}

// Performance test with large input
static void test_extract_nested_string_field_large_input(void **state) {
    (void) state;
    
    // Create a large protobuf-like structure
    uint8_t large_proto[4096];
    memset(large_proto, 0x08, sizeof(large_proto));  // Fill with varint fields
    
    // Add our target field at the end
    size_t offset = sizeof(large_proto) - sizeof(valid_nested_proto);
    memcpy(large_proto + offset, valid_nested_proto, sizeof(valid_nested_proto));
    
    char output[256];
    bool result = extract_nested_string_field(large_proto, 
                                           sizeof(large_proto), 
                                           14, 
                                           output, 
                                           sizeof(output));
    
    assert_true(result);
    assert_string_equal(output, "Test Value");
}

int main(void) {
    const struct CMUnitTest tests[] = {
        // parse_field_tag tests
        cmocka_unit_test(test_parse_field_tag_valid),
        cmocka_unit_test(test_parse_field_tag_invalid_wire_type),
        cmocka_unit_test(test_parse_field_tag_truncated),
        
        // extract_nested_string_field tests
        cmocka_unit_test(test_extract_nested_string_field_valid),
        cmocka_unit_test(test_extract_nested_string_field_not_found),
        cmocka_unit_test(test_extract_nested_string_field_buffer_too_small),
        cmocka_unit_test(test_extract_nested_string_field_malformed_data),
        cmocka_unit_test(test_extract_nested_string_field_empty_input),
        cmocka_unit_test(test_extract_nested_string_field_null_output),
        cmocka_unit_test(test_extract_nested_string_field_deeply_nested),
        cmocka_unit_test(test_extract_nested_string_field_large_input),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
} 