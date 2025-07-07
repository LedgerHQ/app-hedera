#include "time_format.h"

#include "hedera_format.h" // For hedera_snprintf

void format_time_duration(char *buffer, const size_t buffer_size,
                          const uint64_t seconds) {
    uint64_t days = seconds / SECONDS_PER_DAY;
    uint64_t remaining_after_days = seconds % SECONDS_PER_DAY;
    uint64_t hours = remaining_after_days / SECONDS_PER_HOUR;
    uint64_t final_seconds = remaining_after_days % SECONDS_PER_HOUR;

    // Build the string by adding non-zero components
    char temp[200] = {0};
    int pos = 0;

    if (days > 0) {
        pos += hedera_snprintf(temp + pos, sizeof(temp) - pos - 1, "%llu %s",
                               days, days == 1 ? "day" : "days");
    }

    if (hours > 0) {
        if (pos > 0) {
            pos += hedera_snprintf(temp + pos, sizeof(temp) - pos - 1, " ");
        }
        pos += hedera_snprintf(temp + pos, sizeof(temp) - pos - 1, "%llu %s",
                               hours, hours == 1 ? "hour" : "hours");
    }

    if (final_seconds > 0 || (days == 0 && hours == 0)) {
        // Show seconds if there are remaining seconds, or if it's the only
        // component
        if (pos > 0) {
            pos += hedera_snprintf(temp + pos, sizeof(temp) - pos - 1, " ");
        }
        hedera_snprintf(temp + pos, sizeof(temp) - pos - 1, "%llu %s",
                        final_seconds,
                        final_seconds == 1 ? "second" : "seconds");
    }
    PRINTF("temp: %s\n", temp);
    PRINTF("buffer_size: %d\n", buffer_size);
    PRINTF("buffer: %s\n", buffer);
    // Copy a result to the output buffer
    hedera_snprintf(buffer, buffer_size - 1, "%s", temp);
}