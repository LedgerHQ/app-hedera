#include <string.h>
#include <stdint.h>
#include <stddef.h>

#include "proto_varlen_parser.h"

// Maximum reasonable input size for protobuf fuzzing
#define MAX_PROTO_SIZE 8192

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    char output[256];
    
    // Reject inputs that are too large to avoid timeouts
    if (size > MAX_PROTO_SIZE) {
        return 0;
    }
    
    // Reject empty inputs
    if (size == 0) {
        return 0;
    }
    
    // Test field parsing with random field numbers
    if (size >= 4) {
        // Extract field number from first bytes (cast to prevent overflow)
        uint32_t field_number = (uint32_t)(((uint32_t)data[0] << 24) | ((uint32_t)data[1] << 16) | ((uint32_t)data[2] << 8) | (uint32_t)data[3]);
        
        // Limit field number to reasonable range
        field_number = field_number % 1000;
        
        // Test extract_nested_string_field with various field numbers
        extract_nested_string_field(data + 4, size - 4, field_number, output, sizeof(output));
    }
    
    // Test parse_field_tag functionality
    if (size >= 1) {
        const uint8_t *ptr = data;
        const uint8_t *end = data + size;
        protobuf_field_t field;
        
        // Try parsing field tags from different positions
        for (size_t offset = 0; offset < size && offset < 16; offset++) {
            ptr = data + offset;
            if (ptr < end) {
                parse_field_tag(&ptr, end, &field);
            }
        }
    }
    
    // Test with various target field numbers (common Hedera fields)
    uint32_t test_fields[] = {1, 2, 3, 14, 15, 16, 20, 25};
    size_t num_test_fields = sizeof(test_fields) / sizeof(test_fields[0]);
    
    for (size_t i = 0; i < num_test_fields; i++) {
        extract_nested_string_field(data, size, test_fields[i], output, sizeof(output));
    }
    
    // Test edge cases with different output buffer sizes
    char small_output[4];
    char large_output[1024];
    
    extract_nested_string_field(data, size, 14, small_output, sizeof(small_output));
    extract_nested_string_field(data, size, 14, large_output, sizeof(large_output));
    
    return 0;
} 