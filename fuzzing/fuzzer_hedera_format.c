#include <string.h>
#include <stdint.h>
#include <stddef.h>

#include "hedera_format.h"
#include "hedera.h"

#define MAX_FORMAT_SIZE 4096

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    char output[512];
    
    // Reject inputs that are too large
    if (size > MAX_FORMAT_SIZE) {
        return 0;
    }
    
    if (size == 0) {
        return 0;
    }
    
    // Test amount formatting with different values
    if (size >= 8) {
        uint64_t amount = 0;
        memcpy(&amount, data, 8);
        format_hbar(amount, output, sizeof(output));
    }
    
    // Test account ID formatting
    if (size >= 12) {
        hedera_account_id_t account;
        account.shard = (uint64_t)((data[0] << 8) | data[1]);
        account.realm = (uint64_t)((data[2] << 8) | data[3]);
        account.account = (uint64_t)((data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7]);
        
        format_account_id(&account, output, sizeof(output));
    }
    
    // Test timestamp formatting
    if (size >= 8) {
        hedera_timestamp_t timestamp;
        timestamp.seconds = (uint64_t)((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);
        timestamp.nanos = (uint32_t)((data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7]);
        
        format_timestamp(&timestamp, output, sizeof(output));
    }
    
    // Test token ID formatting
    if (size >= 12) {
        hedera_token_id_t token;
        token.shard = (uint64_t)((data[0] << 8) | data[1]);
        token.realm = (uint64_t)((data[2] << 8) | data[3]);
        token.token = (uint64_t)((data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7]);
        
        format_token_id(&token, output, sizeof(output));
    }
    
    // Test duration formatting
    if (size >= 8) {
        hedera_duration_t duration;
        duration.seconds = (uint64_t)((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);
        duration.nanos = (uint32_t)((data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7]);
        
        format_duration(&duration, output, sizeof(output));
    }
    
    // Test with edge cases and different buffer sizes
    char small_output[8];
    char large_output[1024];
    
    if (size >= 8) {
        uint64_t amount = *((uint64_t*)data);
        format_hbar(amount, small_output, sizeof(small_output));
        format_hbar(amount, large_output, sizeof(large_output));
    }
    
    return 0;
} 