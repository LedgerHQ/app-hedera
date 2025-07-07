#include <string.h>
#include <stdint.h>
#include <stddef.h>

#include "staking.h"

#define MAX_STAKING_SIZE 2048

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    char output[256];
    
    // Reject inputs that are too large
    if (size > MAX_STAKING_SIZE) {
        return 0;
    }
    
    if (size == 0) {
        return 0;
    }
    
    // Test staking info formatting with different values
    if (size >= 16) {
        hedera_staking_info_t staking_info;
        
        // Extract staking data from input
        staking_info.decline_reward = (data[0] & 0x01) ? true : false;
        staking_info.staked_account_id = (int64_t)((data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4]);
        staking_info.staked_node_id = (int64_t)((data[5] << 24) | (data[6] << 16) | (data[7] << 8) | data[8]);
        
        // Test different formatting scenarios
        format_staking_info(&staking_info, output, sizeof(output));
    }
    
    // Test with various edge cases
    if (size >= 8) {
        hedera_staking_info_t edge_cases[] = {
            {true, -1, -1},     // Decline reward with invalid IDs
            {false, 0, 0},      // No decline, zero IDs
            {true, LLONG_MAX, LLONG_MIN},  // Extreme values
            {false, 123456789, -987654321} // Mixed positive/negative
        };
        
        for (size_t i = 0; i < sizeof(edge_cases) / sizeof(edge_cases[0]); i++) {
            format_staking_info(&edge_cases[i], output, sizeof(output));
        }
    }
    
    // Test with different buffer sizes
    char small_output[16];
    char large_output[512];
    
    if (size >= 8) {
        hedera_staking_info_t staking;
        staking.decline_reward = data[0] & 0x01;
        staking.staked_account_id = (int64_t)((data[1] << 8) | data[2]);
        staking.staked_node_id = (int64_t)((data[3] << 8) | data[4]);
        
        format_staking_info(&staking, small_output, sizeof(small_output));
        format_staking_info(&staking, large_output, sizeof(large_output));
    }
    
    // Test validation functions if available
    if (size >= 4) {
        int64_t account_id = (int64_t)((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);
        int64_t node_id = (int64_t)((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);
        
        // Test validation (assuming these functions exist)
        validate_staking_account_id(account_id);
        validate_staking_node_id(node_id);
    }
    
    return 0;
} 