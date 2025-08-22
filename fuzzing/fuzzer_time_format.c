#include <string.h>
#include <stdint.h>
#include <stddef.h>

#include "time_format.h"

#define MAX_TIME_SIZE 1024

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    char output[128];
    
    // Reject inputs that are too large
    if (size > MAX_TIME_SIZE) {
        return 0;
    }
    
    if (size == 0) {
        return 0;
    }
    
    // Test timestamp formatting with different values
    if (size >= 8) {
        hedera_timestamp_t timestamp;
        timestamp.seconds = (uint64_t)((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);
        timestamp.nanos = (uint32_t)((data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7]);
        
        format_timestamp(&timestamp, output, sizeof(output));
    }
    
    // Test duration formatting
    if (size >= 8) {
        hedera_duration_t duration;
        duration.seconds = (uint64_t)((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);
        duration.nanos = (uint32_t)((data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7]);
        
        format_duration(&duration, output, sizeof(output));
    }
    
    // Test edge cases with extreme values
    if (size >= 16) {
        hedera_timestamp_t edge_timestamps[] = {
            {0, 0},                    // Epoch
            {UINT64_MAX, UINT32_MAX},  // Maximum values
            {1234567890, 999999999},   // Valid timestamp
            {0, UINT32_MAX},           // Zero seconds, max nanos
            {UINT64_MAX, 0}            // Max seconds, zero nanos
        };
        
        for (size_t i = 0; i < sizeof(edge_timestamps) / sizeof(edge_timestamps[0]); i++) {
            format_timestamp(&edge_timestamps[i], output, sizeof(output));
        }
        
        hedera_duration_t edge_durations[] = {
            {0, 0},                    // Zero duration
            {UINT64_MAX, UINT32_MAX},  // Maximum duration
            {3600, 0},                 // One hour
            {86400, 0},                // One day
            {0, 500000000}             // Half second
        };
        
        for (size_t i = 0; i < sizeof(edge_durations) / sizeof(edge_durations[0]); i++) {
            format_duration(&edge_durations[i], output, sizeof(output));
        }
    }
    
    // Test with different buffer sizes
    char small_output[8];
    char large_output[256];
    
    if (size >= 8) {
        hedera_timestamp_t timestamp;
        timestamp.seconds = (uint64_t)((data[0] << 8) | data[1]);
        timestamp.nanos = (uint32_t)((data[2] << 8) | data[3]);
        
        format_timestamp(&timestamp, small_output, sizeof(small_output));
        format_timestamp(&timestamp, large_output, sizeof(large_output));
    }
    
    // Test timestamp validation functions if available
    if (size >= 8) {
        hedera_timestamp_t timestamp;
        timestamp.seconds = (uint64_t)((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);
        timestamp.nanos = (uint32_t)((data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7]);
        
        // Test validation (assuming these functions exist)
        validate_timestamp(&timestamp);
        is_valid_timestamp(&timestamp);
    }
    
    return 0;
} 