#include <limits.h>
#include <string.h>

#include "../utils.h"
#include "swap_token_utils.h"

bool swap_str_to_u64(const uint8_t* src, size_t length, uint64_t* result) {
    if (length > sizeof(uint64_t)) {
        return false;
    }
    uint64_t value = 0;
    for (size_t i = 0; i < length; i++) {
        value <<= 8;
        value |= src[i];
    }
    *result = value;
    return true;
}

int print_token_amount(uint64_t amount,
                       const char *const asset,
                       uint8_t decimals,
                       char *out,
                       const size_t out_length) {
    BAIL_IF(out_length > INT_MAX);
    uint64_t dVal = amount;
    const int outlen = (int) out_length;
    int i = 0;
    int min_chars = decimals + 1;

    if (i < (outlen - 1)) {
        do {
            if (i == decimals) {
                out[i] = '.';
                i += 1;
            }
            out[i] = (dVal % 10) + '0';
            dVal /= 10;
            i += 1;
        } while ((dVal > 0 || i < min_chars) && i < outlen);
    }
    BAIL_IF(i >= outlen);
    // Reverse order
    int j, k;
    for (j = 0, k = i - 1; j < k; j++, k--) {
        char tmp = out[j];
        out[j] = out[k];
        out[k] = tmp;
    }
    // Strip trailing 0s
    for (i -= 1; i > 0; i--) {
        if (out[i] != '0') break;
    }
    i += 1;

    // Strip trailing .
    if (out[i - 1] == '.') i -= 1;

    if (asset) {
        const int asset_length = strlen(asset);
        // Check buffer has space
        BAIL_IF((i + 1 + asset_length + 1) > outlen);
        // Qualify amount
        out[i++] = ' ';
        strncpy(out + i, asset, asset_length + 1);
    } else {
        out[i] = '\0';
    }

    return 0;
}
