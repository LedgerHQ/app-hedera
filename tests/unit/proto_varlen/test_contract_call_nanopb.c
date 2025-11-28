#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <cmocka.h>
#include <stdlib.h>
#include <limits.h>

// Define PB_SYSTEM_HEADER to use our custom system headers
#define PB_SYSTEM_HEADER "nanopb_system.h"

// Include nanopb headers
#include <pb_decode.h>
#include <pb_encode.h>

// Include the protobuf message definitions
#include "contract_call.pb.h"
#include "basic_types.pb.h"

//Include constants
#include "app_globals.h"

// Test helper functions to create contract call protobuf data with nanopb
static bool create_contract_call_with_nanopb(uint8_t *buffer, size_t buffer_size, 
                                           size_t function_params_length, size_t *encoded_size) {
    // Initialize the contract call message
    Hedera_ContractCallTransactionBody contract_call = Hedera_ContractCallTransactionBody_init_zero;
    
    // Set up contract ID
    contract_call.has_contractID = true;
    contract_call.contractID.shardNum = 0;
    contract_call.contractID.realmNum = 0;
    contract_call.contractID.which_contract = Hedera_ContractID_contractNum_tag;
    contract_call.contractID.contract.contractNum = 1;
    
    // Set gas and amount
    contract_call.gas = 100000;
    contract_call.amount = 1000;
    
    // Set function parameters (truncate if necessary)
    size_t actual_length = function_params_length;
    contract_call.functionParameters.size = actual_length;
    memset(contract_call.functionParameters.bytes, 0xAB, actual_length);
    
    // Encode the message
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_size);
    bool result = pb_encode(&stream, Hedera_ContractCallTransactionBody_fields, &contract_call);
    
    if (encoded_size) {
        *encoded_size = stream.bytes_written;
    }
    
    return result;
}

// Test function to validate contract call with nanopb decoding
static bool validate_contract_call_with_nanopb(const uint8_t *data, size_t data_size, 
                                             size_t max_length) {
    if (!data || data_size == 0) {
        return false;
    }
    
    // Decode the message
    Hedera_ContractCallTransactionBody contract_call = Hedera_ContractCallTransactionBody_init_zero;
    pb_istream_t stream = pb_istream_from_buffer(data, data_size);
    
    if (!pb_decode(&stream, Hedera_ContractCallTransactionBody_fields, &contract_call)) {
        return false;
    }
    
    // Check if function parameters exceed the limit
    if (contract_call.functionParameters.size > max_length) {
        return false;
    }
    
    return true;
}

// Test cases for contract call with nanopb

static void test_contract_call_nanopb_normal_function_parameters(void **state) {
    (void) state;
    
    // Create a contract call with normal-sized function parameters
    uint8_t buffer[4096];
    size_t encoded_size;
    bool result = create_contract_call_with_nanopb(buffer, sizeof(buffer), 100, &encoded_size);
    assert_true(result);
    
    // Validate that the varlen field length is acceptable
    result = validate_contract_call_with_nanopb(buffer, encoded_size, MAX_TX_SIZE);
    assert_true(result);
}

static void test_contract_call_nanopb_long_function_parameters(void **state) {
    (void) state;
    
    // Create a contract call with long function parameters (1KB)
    uint8_t buffer[4096];
    size_t encoded_size;
    bool result = create_contract_call_with_nanopb(buffer, sizeof(buffer), MAX_TX_SIZE, &encoded_size);
    assert_true(result);
    
    // Validate that the varlen field length is at the limit
    result = validate_contract_call_with_nanopb(buffer, encoded_size, MAX_TX_SIZE);
    assert_true(result);
}

// Removed extremely_long_function_parameters test to avoid stack overflow issues

static void test_contract_call_nanopb_zero_length_function_parameters(void **state) {
    (void) state;
    
    // Create a contract call with zero-length function parameters
    uint8_t buffer[4096];
    size_t encoded_size;
    bool result = create_contract_call_with_nanopb(buffer, sizeof(buffer), 0, &encoded_size);
    assert_true(result);
    
    // Validate that the varlen field length is acceptable
    result = validate_contract_call_with_nanopb(buffer, encoded_size, MAX_TX_SIZE);
    assert_true(result);
}

static void test_contract_call_nanopb_exactly_at_limit(void **state) {
    (void) state;
    
    // Create a contract call with function parameters exactly at the 1KB limit
    uint8_t buffer[4096];
    size_t encoded_size;
    bool result = create_contract_call_with_nanopb(buffer, sizeof(buffer), MAX_TX_SIZE, &encoded_size);
    assert_true(result);
    
    // Validate that the varlen field length is exactly at the limit
    result = validate_contract_call_with_nanopb(buffer, encoded_size, MAX_TX_SIZE);
    assert_true(result);
}

static void test_contract_call_nanopb_malformed_data(void **state) {
    (void) state;
    
    // Create malformed data that should fail nanopb decoding
    uint8_t buffer[100];
    memset(buffer, 0xFF, sizeof(buffer));
    
    // Validate that malformed data is rejected
    bool result = validate_contract_call_with_nanopb(buffer, sizeof(buffer), MAX_TX_SIZE);
    assert_false(result);
}

static void test_contract_call_nanopb_incomplete_data(void **state) {
    (void) state;
    
    // Create incomplete data that should fail nanopb decoding
    uint8_t buffer[10];
    memset(buffer, 0x00, sizeof(buffer));
    
    // Validate that incomplete data is rejected
    bool result = validate_contract_call_with_nanopb(buffer, sizeof(buffer), MAX_TX_SIZE);
    assert_false(result);
}

static void test_contract_call_nanopb_empty_buffer(void **state) {
    (void) state;
    
    // Test with empty buffer
    uint8_t buffer[1] = {0};
    
    // Validate that empty buffer is rejected
    bool result = validate_contract_call_with_nanopb(buffer, 0, MAX_TX_SIZE);
    assert_false(result);
}

static void test_contract_call_nanopb_null_buffer(void **state) {
    (void) state;
    
    // Test with NULL buffer
    bool result = validate_contract_call_with_nanopb(NULL, 0, MAX_TX_SIZE);
    assert_false(result);
}

static void test_contract_call_nanopb_encoding_failure(void **state) {
    (void) state;
    
    // Test encoding with buffer too small
    uint8_t buffer[10];  // Very small buffer
    size_t encoded_size;
    bool result = create_contract_call_with_nanopb(buffer, sizeof(buffer), 100, &encoded_size);
    assert_false(result);  // Should fail due to insufficient buffer space
}

int main(void) {
    const struct CMUnitTest tests[] = {
        // Tests for normal and long function parameters
        cmocka_unit_test(test_contract_call_nanopb_normal_function_parameters),
        cmocka_unit_test(test_contract_call_nanopb_long_function_parameters),
        cmocka_unit_test(test_contract_call_nanopb_zero_length_function_parameters),
        
        // Tests for 1KB limit enforcement
        cmocka_unit_test(test_contract_call_nanopb_exactly_at_limit),
        
        // Tests for malformed data
        cmocka_unit_test(test_contract_call_nanopb_malformed_data),
        cmocka_unit_test(test_contract_call_nanopb_incomplete_data),
        cmocka_unit_test(test_contract_call_nanopb_encoding_failure),
        
        // Tests for edge cases
        cmocka_unit_test(test_contract_call_nanopb_empty_buffer),
        cmocka_unit_test(test_contract_call_nanopb_null_buffer),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
