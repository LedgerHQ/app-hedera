#pragma once

#include <stdint.h>
#include <stddef.h>

// Time constants
#define SECONDS_PER_MINUTE 60
#define SECONDS_PER_HOUR   3600
#define SECONDS_PER_DAY    86400

/**
 * Universal time formatting function - handles all cases automatically
 * Formats time as hierarchical "X days Y hours Z seconds" showing only non-zero components
 * @param buffer Output buffer
 * @param buffer_size Size of output buffer
 * @param seconds Total seconds to format
 */
void format_time_duration(char *buffer, size_t buffer_size, uint64_t seconds);