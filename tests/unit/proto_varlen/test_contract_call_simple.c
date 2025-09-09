#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <cmocka.h>
#include <stdlib.h>
#include <limits.h>

// Simple test for contract call varlen field handling without nanopb
// This test focuses on the core logic of handling long varlen fields

// Test helper functions to create contract call protobuf data with long varlen fields
static void create_contract_call_with_long_function_parameters(uint8_t *buffer, size_t buffer_size, 
                                                              size_t function_params_length) {
    uint8_t *ptr = buffer;
    size_t remaining = buffer_size;
    
    // Create ContractCallTransactionBody message
    // Field 1: contractID (optional message)
    if (remaining < 2) return;
    *ptr++ = (1 << 3) | 2;  // Field 1, wire type 2 (message)
    remaining--;
    
    // ContractID submessage length (minimal)
    if (remaining < 2) return;
    *ptr++ = 2;  // Length 2
    remaining--;
    
    // ContractID fields (minimal)
    if (remaining < 2) return;
    *ptr++ = (1 << 3) | 0;  // Field 1, wire type 0 (varint)
    *ptr++ = 1;  // Value 1
    remaining -= 2;
    
    // Field 2: gas (int64)
    if (remaining < 3) return;
    *ptr++ = (2 << 3) | 0;  // Field 2, wire type 0 (varint)
    // Write varint for 100000
    uint64_t gas_value = 100000;
    while (gas_value >= 0x80) {
        *ptr++ = (uint8_t)(gas_value | 0x80);
        gas_value >>= 7;
        remaining--;
    }
    *ptr++ = (uint8_t)gas_value;
    remaining--;
    
    // Field 3: amount (int64)
    if (remaining < 3) return;
    *ptr++ = (3 << 3) | 0;  // Field 3, wire type 0 (varint)
    // Write varint for 1000
    uint64_t amount_value = 1000;
    while (amount_value >= 0x80) {
        *ptr++ = (uint8_t)(amount_value | 0x80);
        amount_value >>= 7;
        remaining--;
    }
    *ptr++ = (uint8_t)amount_value;
    remaining--;
    
    // Field 4: functionParameters (bytes) - this is the varlen field we want to test
    if (remaining < 2) return;
    *ptr++ = (4 << 3) | 2;  // Field 4, wire type 2 (bytes)
    remaining--;
    
    // Write function parameters length (keep original length for validation)
    if (remaining < 5) return;  // Need space for varint
    size_t varint_len = 0;
    uint64_t len = function_params_length;
    
    while (len >= 0x80) {
        *ptr++ = (uint8_t)(len | 0x80);
        len >>= 7;
        varint_len++;
        remaining--;
    }
    *ptr++ = (uint8_t)len;
    varint_len++;
    remaining--;
    
    // Write function parameters data (truncated if necessary)
    size_t data_len = (remaining < function_params_length) ? remaining : function_params_length;
    if (data_len > 0) {
        memset(ptr, 0xAB, data_len);  // Fill with test pattern
        ptr += data_len;
    }
}

// Test function to validate varlen field length
static bool validate_varlen_field_length(const uint8_t *data, size_t data_size, size_t max_length) {
    if (!data || data_size == 0) {
        return false;
    }
    
    const uint8_t *ptr = data;
    const uint8_t *end = data + data_size;
    
    // Look for field 4 (functionParameters) - 0x22 = (4 << 3) | 2
    while (ptr < end) {
        if (*ptr == 0x22) {  // Field 4, wire type 2
            ptr++;
            
            // Read the length varint
            uint64_t length = 0;
            uint8_t shift = 0;
            while (ptr < end && (*ptr & 0x80)) {
                if (shift >= 64) return false;  // Overflow protection
                length |= (uint64_t)(*ptr & 0x7F) << shift;
                shift += 7;
                ptr++;
            }
            if (ptr >= end) return false;  // Incomplete varint
            
            length |= (uint64_t)(*ptr & 0x7F) << shift;
            ptr++;
            
            // Check if length exceeds maximum
            if (length > max_length) {
                return false;
            }
            
            // Check if we have enough data
            if (ptr + length > end) {
                return false;  // Incomplete data
            }
            
            return true;  // Found field 4 and validated length
        }
        
        // Skip current field
        uint8_t field_tag = *ptr++;
        uint8_t wire_type = field_tag & 0x07;
        
        if (wire_type == 0) {  // varint
            while (ptr < end && (*ptr & 0x80)) {
                ptr++;
            }
            if (ptr < end) ptr++;
        } else if (wire_type == 2) {  // bytes/string
            uint64_t length = 0;
            uint8_t shift = 0;
            while (ptr < end && (*ptr & 0x80)) {
                if (shift >= 64) return false;  // Overflow protection
                length |= (uint64_t)(*ptr & 0x7F) << shift;
                shift += 7;
                ptr++;
            }
            if (ptr < end) {
                length |= (uint64_t)(*ptr & 0x7F) << shift;
                ptr++;
            }
            
            // Skip the actual data
            if (ptr + length > end) {
                return false;  // Incomplete data
            }
            ptr += length;
        } else {
            return false;  // Unsupported wire type
        }
    }
    
    return false;  // Field 4 not found
}

// Test cases for contract call with long varlen fields

static void test_contract_call_normal_function_parameters(void **state) {
    (void) state;
    
    // Create a contract call with normal-sized function parameters
    uint8_t buffer[1024];
    create_contract_call_with_long_function_parameters(buffer, sizeof(buffer), 100);
    
    // Calculate actual data size used
    size_t actual_size = 0;
    for (size_t i = 0; i < sizeof(buffer); i++) {
        if (buffer[i] != 0) actual_size = i + 1;
    }
    
    // Validate that the varlen field length is acceptable
    bool result = validate_varlen_field_length(buffer, actual_size, 1024);
    
    assert_true(result);
}

static void test_contract_call_long_function_parameters(void **state) {
    (void) state;
    
    // Create a contract call with long function parameters (1KB)
    uint8_t buffer[2048];
    create_contract_call_with_long_function_parameters(buffer, sizeof(buffer), 1024);
    
    // Validate that the varlen field length is at the limit
    // Calculate actual data size used
    size_t actual_size = 0;
    for (size_t i = 0; i < sizeof(buffer); i++) {
        if (buffer[i] != 0) actual_size = i + 1;
    }
    
    bool result = validate_varlen_field_length(buffer, actual_size, 1024);
    
    assert_true(result);
}

static void test_contract_call_extremely_long_function_parameters(void **state) {
    (void) state;
    
    // Create a contract call with extremely long function parameters
    uint8_t buffer[2048];
    create_contract_call_with_long_function_parameters(buffer, sizeof(buffer), 10000);
    
    // Validate that the varlen field length exceeds the limit
    // Calculate actual data size used
    size_t actual_size = 0;
    for (size_t i = 0; i < sizeof(buffer); i++) {
        if (buffer[i] != 0) actual_size = i + 1;
    }
    
    bool result = validate_varlen_field_length(buffer, actual_size, 1024);
    
    // Should fail due to extremely long length
    assert_false(result);
}

static void test_contract_call_overflow_function_parameters(void **state) {
    (void) state;
    
    // Create a contract call with function parameters that exceed the 1KB limit
    uint8_t buffer[2048];
    create_contract_call_with_long_function_parameters(buffer, sizeof(buffer), 2000);
    
    // Calculate actual data size used
    size_t actual_size = 0;
    for (size_t i = 0; i < sizeof(buffer); i++) {
        if (buffer[i] != 0) actual_size = i + 1;
    }
    
    // Validate that the varlen field length exceeds the limit
    bool result = validate_varlen_field_length(buffer, actual_size, 1024);
    
    // Should fail because the claimed length (2000) exceeds the limit (1024)
    assert_false(result);
}

static void test_contract_call_boundary_function_parameters(void **state) {
    (void) state;
    
    // Create a contract call with function parameters at the exact boundary
    uint8_t buffer[200];
    create_contract_call_with_long_function_parameters(buffer, sizeof(buffer), 100);
    
    // Calculate actual data size used
    size_t actual_size = 0;
    for (size_t i = 0; i < sizeof(buffer); i++) {
        if (buffer[i] != 0) actual_size = i + 1;
    }
    
    // Validate that the varlen field length is acceptable
    bool result = validate_varlen_field_length(buffer, actual_size, 1024);
    
    assert_true(result);
}

static void test_contract_call_zero_length_function_parameters(void **state) {
    (void) state;
    
    // Create a contract call with zero-length function parameters
    uint8_t buffer[200];
    create_contract_call_with_long_function_parameters(buffer, sizeof(buffer), 0);
    
    // Calculate actual data size used
    size_t actual_size = 0;
    for (size_t i = 0; i < sizeof(buffer); i++) {
        if (buffer[i] != 0) actual_size = i + 1;
    }
    
    // Validate that the varlen field length is acceptable
    bool result = validate_varlen_field_length(buffer, actual_size, 1024);
    
    assert_true(result);
}

static void test_contract_call_malformed_length(void **state) {
    (void) state;
    
    // Create a malformed contract call with invalid length encoding
    uint8_t buffer[200];
    uint8_t *ptr = buffer;
    
    // Field 1: contractID (minimal)
    *ptr++ = (1 << 3) | 2;
    *ptr++ = 2;
    *ptr++ = (1 << 3) | 0;
    *ptr++ = 1;
    
    // Field 2: gas
    *ptr++ = (2 << 3) | 0;
    // Write varint for 100000
    uint64_t gas_value = 100000;
    while (gas_value >= 0x80) {
        *ptr++ = (uint8_t)(gas_value | 0x80);
        gas_value >>= 7;
    }
    *ptr++ = (uint8_t)gas_value;
    
    // Field 3: amount
    *ptr++ = (3 << 3) | 0;
    // Write varint for 1000
    uint64_t amount_value = 1000;
    while (amount_value >= 0x80) {
        *ptr++ = (uint8_t)(amount_value | 0x80);
        amount_value >>= 7;
    }
    *ptr++ = (uint8_t)amount_value;
    
    // Field 4: functionParameters with malformed varint length
    *ptr++ = (4 << 3) | 2;
    
    // Write malformed varint (all continuation bits set)
    for (int i = 0; i < 10; i++) {
        *ptr++ = 0xFF;
    }
    *ptr++ = 0x7F;  // Final byte
    
    // Validate that the malformed data is rejected
    // Calculate actual data size used
    size_t actual_size = 0;
    for (size_t i = 0; i < sizeof(buffer); i++) {
        if (buffer[i] != 0) actual_size = i + 1;
    }
    
    bool result = validate_varlen_field_length(buffer, actual_size, 1024);
    
    assert_false(result);
}

static void test_contract_call_incomplete_function_parameters(void **state) {
    (void) state;
    
    // Create a contract call with incomplete function parameters
    uint8_t buffer[200];
    uint8_t *ptr = buffer;
    
    // Field 1: contractID (minimal)
    *ptr++ = (1 << 3) | 2;
    *ptr++ = 2;
    *ptr++ = (1 << 3) | 0;
    *ptr++ = 1;
    
    // Field 2: gas
    *ptr++ = (2 << 3) | 0;
    // Write varint for 100000
    uint64_t gas_value = 100000;
    while (gas_value >= 0x80) {
        *ptr++ = (uint8_t)(gas_value | 0x80);
        gas_value >>= 7;
    }
    *ptr++ = (uint8_t)gas_value;
    
    // Field 3: amount
    *ptr++ = (3 << 3) | 0;
    // Write varint for 1000
    uint64_t amount_value = 1000;
    while (amount_value >= 0x80) {
        *ptr++ = (uint8_t)(amount_value | 0x80);
        amount_value >>= 7;
    }
    *ptr++ = (uint8_t)amount_value;
    
    // Field 4: functionParameters with incomplete data
    *ptr++ = (4 << 3) | 2;
    *ptr++ = 50;  // Claim 50 bytes
    
    // Only write 25 bytes of data (incomplete)
    memset(ptr, 0xAA, 25);
    ptr += 25;
    
    // Validate that the incomplete data is rejected
    // Truncate the buffer to make it incomplete
    size_t actual_size = ptr - buffer;
    
    bool result = validate_varlen_field_length(buffer, actual_size, 1024);
    
    assert_false(result);
}

static void test_contract_call_max_uint64_length(void **state) {
    (void) state;
    
    // Create a contract call with UINT64_MAX length for function parameters
    uint8_t buffer[200];
    uint8_t *ptr = buffer;
    
    // Field 1: contractID (minimal)
    *ptr++ = (1 << 3) | 2;
    *ptr++ = 2;
    *ptr++ = (1 << 3) | 0;
    *ptr++ = 1;
    
    // Field 2: gas
    *ptr++ = (2 << 3) | 0;
    // Write varint for 100000
    uint64_t gas_value = 100000;
    while (gas_value >= 0x80) {
        *ptr++ = (uint8_t)(gas_value | 0x80);
        gas_value >>= 7;
    }
    *ptr++ = (uint8_t)gas_value;
    
    // Field 3: amount
    *ptr++ = (3 << 3) | 0;
    // Write varint for 1000
    uint64_t amount_value = 1000;
    while (amount_value >= 0x80) {
        *ptr++ = (uint8_t)(amount_value | 0x80);
        amount_value >>= 7;
    }
    *ptr++ = (uint8_t)amount_value;
    
    // Field 4: functionParameters with UINT64_MAX length
    *ptr++ = (4 << 3) | 2;
    
    // Write UINT64_MAX length
    uint64_t max_len = UINT64_MAX;
    while (max_len >= 0x80) {
        *ptr++ = (uint8_t)(max_len | 0x80);
        max_len >>= 7;
    }
    *ptr++ = (uint8_t)max_len;
    
    // Write some data
    memset(ptr, 0xBB, 10);
    ptr += 10;
    
    // Validate that the extremely large length is rejected
    // Calculate actual data size used
    size_t actual_size = 0;
    for (size_t i = 0; i < sizeof(buffer); i++) {
        if (buffer[i] != 0) actual_size = i + 1;
    }
    
    bool result = validate_varlen_field_length(buffer, actual_size, 1024);
    
    assert_false(result);
}

static void test_contract_call_empty_buffer(void **state) {
    (void) state;
    
    // Test with empty buffer
    uint8_t buffer[1] = {0};
    
    // Validate that empty buffer is rejected
    bool result = validate_varlen_field_length(buffer, 0, 1024);
    
    assert_false(result);
}

static void test_contract_call_null_buffer(void **state) {
    (void) state;
    
    // Test with NULL buffer
    bool result = validate_varlen_field_length(NULL, 0, 1024);
    
    assert_false(result);
}

static void test_contract_call_one_byte_too_long(void **state) {
    (void) state;
    
    // Create a contract call with function parameters one byte too long
    uint8_t buffer[100];
    create_contract_call_with_long_function_parameters(buffer, sizeof(buffer), 50);
    
    // Validate that the varlen field length exceeds the limit by one byte
    // Calculate actual data size used
    size_t actual_size = 0;
    for (size_t i = 0; i < sizeof(buffer); i++) {
        if (buffer[i] != 0) actual_size = i + 1;
    }
    
    bool result = validate_varlen_field_length(buffer, actual_size, 49);
    
    assert_false(result);
}

static void test_contract_call_exactly_at_limit(void **state) {
    (void) state;
    
    // Create a contract call with function parameters exactly at the 1KB limit
    uint8_t buffer[2048];
    create_contract_call_with_long_function_parameters(buffer, sizeof(buffer), 1024);
    
    // Validate that the varlen field length is exactly at the limit
    // Calculate actual data size used
    size_t actual_size = 0;
    for (size_t i = 0; i < sizeof(buffer); i++) {
        if (buffer[i] != 0) actual_size = i + 1;
    }
    
    bool result = validate_varlen_field_length(buffer, actual_size, 1024);
    
    assert_true(result);
}

static void test_contract_call_just_over_limit(void **state) {
    (void) state;
    
    // Create a contract call with function parameters just over the 1KB limit
    uint8_t buffer[2048];
    create_contract_call_with_long_function_parameters(buffer, sizeof(buffer), 1025);
    
    // Validate that the varlen field length exceeds the limit
    // Calculate actual data size used
    size_t actual_size = 0;
    for (size_t i = 0; i < sizeof(buffer); i++) {
        if (buffer[i] != 0) actual_size = i + 1;
    }
    
    bool result = validate_varlen_field_length(buffer, actual_size, 1024);
    
    // Should fail because the claimed length (1025) exceeds the limit (1024)
    assert_false(result);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        // Tests for normal and long function parameters
        cmocka_unit_test(test_contract_call_normal_function_parameters),
        cmocka_unit_test(test_contract_call_long_function_parameters),
        cmocka_unit_test(test_contract_call_extremely_long_function_parameters),
        cmocka_unit_test(test_contract_call_overflow_function_parameters),
        cmocka_unit_test(test_contract_call_boundary_function_parameters),
        cmocka_unit_test(test_contract_call_zero_length_function_parameters),
        
        // Tests for malformed data
        cmocka_unit_test(test_contract_call_malformed_length),
        cmocka_unit_test(test_contract_call_incomplete_function_parameters),
        cmocka_unit_test(test_contract_call_max_uint64_length),
        cmocka_unit_test(test_contract_call_one_byte_too_long),
        
        // Tests for edge cases
        cmocka_unit_test(test_contract_call_empty_buffer),
        cmocka_unit_test(test_contract_call_null_buffer),
        
        // Tests for 1KB limit enforcement
        cmocka_unit_test(test_contract_call_exactly_at_limit),
        cmocka_unit_test(test_contract_call_just_over_limit),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
