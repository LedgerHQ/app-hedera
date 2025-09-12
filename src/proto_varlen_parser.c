#include "proto_varlen_parser.h"

#include <string.h>

// Forward declaration
static bool skip_field(const uint8_t **data, const uint8_t *end,
                       uint32_t wire_type);

// Decode a varint from protobuf data (second-stage decoding)
static bool decode_varint(const uint8_t **data, const uint8_t *end,
                          uint64_t *result) {
    uint64_t value = 0;
    uint8_t shift = 0;
    uint8_t byte = 0;

    do {
        if (*data >= end) {
            return false;
        }
        if (shift >= 64) {
            return false; // Varint overflow
        }

        byte = **data;
        (*data)++;

        value |= (uint64_t)(byte & 0x7F) << shift;
        shift += 7;
    } while (byte & 0x80);

    *result = value;
    return true;
}

bool parse_field_tag(const uint8_t **data, const uint8_t *end,
                     protobuf_field_t *field) {
    if (!data || !*data || !end || !field) {
        return false;
    }

    uint64_t tag = 0;
    if (!decode_varint(data, end, &tag)) {
        return false;
    }

    field->field_number = (uint32_t)(tag >> 3);
    field->wire_type = (wire_type_t)(tag & 0x07);
    field->data = *data;
    field->length = 0;

    return true;
}

// Helper function to extract string from StringValue submessage
static bool extract_string_from_string_value(const uint8_t *sv_data,
                                             const uint8_t *sv_end,
                                             char *output,
                                             const size_t output_size) {
    if (!sv_data || !sv_end || !output || output_size == 0) {
        return false;
    }
    
    while (sv_data < sv_end) {
        uint64_t sv_tag = 0;
        if (!decode_varint(&sv_data, sv_end, &sv_tag)) {
            return false;
        }

        const uint32_t sv_field_num = (uint32_t)(sv_tag >> 3);
        const uint32_t sv_wire_type = (uint32_t)(sv_tag & 7);

        if (sv_field_num == 1) { // StringValue.value field
            if (sv_wire_type != WIRE_TYPE_STRING) {
                return false;
            }

            uint64_t string_length = 0;
            if (!decode_varint(&sv_data, sv_end, &string_length)) {
                return false;
            }

            // Bounds check without pointer overflow
            size_t remaining = (size_t)(sv_end - sv_data);
            if (string_length > (uint64_t)remaining) {
                return false;
            }

            // Extract string data
            size_t copy_len = (size_t)string_length;
            if (copy_len >= output_size) {
                copy_len = output_size - 1;
            }

            // Find null terminator within the string data
            for (size_t i = 0; i < copy_len; i++) {
                if (sv_data[i] == '\0') {
                    copy_len = i;
                    break;
                }
            }

            memcpy(output, sv_data, copy_len);
            output[copy_len] = '\0';
            return true;
        }

        // Skip other fields in StringValue
        if (!skip_field(&sv_data, sv_end, sv_wire_type)) {
            return false;
        }
    }

    return false; // String value not found
}

// Helper function to parse CryptoUpdateTransactionBody submessage
static bool parse_crypto_update_body(const uint8_t *crypto_data,
                                     const uint8_t *crypto_end,
                                     uint32_t field_number, char *output,
                                     const size_t output_size) {
    if (!crypto_data || !crypto_end || !output || output_size == 0) {
        return false;
    }
    
    while (crypto_data < crypto_end) {
        uint64_t crypto_tag = 0;
        if (!decode_varint(&crypto_data, crypto_end, &crypto_tag)) {
            return false;
        }

        const uint32_t crypto_field_num = (uint32_t)(crypto_tag >> 3);
        const uint32_t crypto_wire_type = (uint32_t)(crypto_tag & 7);

        if (crypto_field_num == field_number) {
            if (crypto_wire_type != WIRE_TYPE_STRING) {
                return false;
            }

            uint64_t string_value_length = 0;
            if (!decode_varint(&crypto_data, crypto_end, &string_value_length)) {
                return false;
            }

            // Bounds check without pointer overflow
            size_t remaining = (size_t)(crypto_end - crypto_data);
            if (string_value_length > (uint64_t)remaining) {
                return false;
            }

            // Parse the StringValue submessage
            const uint8_t *sv_data = crypto_data;
            const uint8_t *sv_end = crypto_data + string_value_length;

            return extract_string_from_string_value(sv_data, sv_end, output,
                                                    output_size);
        }

        // Skip other fields in CryptoUpdateTransactionBody
        if (!skip_field(&crypto_data, crypto_end, crypto_wire_type)) {
            return false;
        }
    }

    return false; // Target field not found
}

// Second-stage protobuf decoder for nested StringValue fields
bool extract_nested_string_field(const uint8_t *buffer, size_t buffer_size,
                                 const uint32_t field_number, char *output,
                                 const size_t output_size) {
    if (!buffer || !output || output_size == 0) {
        return false;
    }

    const uint8_t *data = buffer;
    const uint8_t *end = buffer + buffer_size;

    // Clear output buffer
    memset(output, 0, output_size);

    // Find field 15 (cryptoUpdateAccount) in the TransactionBody
    while (data < end) {
        uint64_t tag = 0;
        if (!decode_varint(&data, end, &tag)) {
            return false;
        }

        const uint32_t field_num = (uint32_t)(tag >> 3);
        const uint32_t wire_type = (uint32_t)(tag & 7);

        if (field_num == 15) { // cryptoUpdateAccount field
            if (wire_type != WIRE_TYPE_STRING) {
                return false;
            }

            uint64_t crypto_update_length = 0;
            if (!decode_varint(&data, end, &crypto_update_length)) {
                return false;
            }

            // Bounds check without pointer overflow
            size_t remaining = (size_t)(end - data);
            if (crypto_update_length > (uint64_t)remaining) {
                return false;
            }

            // Parse the CryptoUpdateTransactionBody submessage
            const uint8_t *crypto_data = data;
            const uint8_t *crypto_end = data + crypto_update_length;

            return parse_crypto_update_body(crypto_data, crypto_end,
                                            field_number, output, output_size);
        }

        // Skip other fields
        if (!skip_field(&data, end, wire_type)) {
            return false;
        }
    }

    return false; // Field not found
}

// Helper function to skip a field based on wire type during second-stage
// decoding
static bool skip_field(const uint8_t **data, const uint8_t *end,
                       const uint32_t wire_type) {
    if (!data || !*data || !end) {
        return false;
    }
    
    switch (wire_type) {
        case WIRE_TYPE_VARINT: {
            uint64_t dummy = 0;
            if (!decode_varint(data, end, &dummy)) {
                return false;
            }
            break;
        }
        case WIRE_TYPE_64BIT:
            if (*data + 8 > end) {
                return false;
            }
            *data += 8;
            break;
        case WIRE_TYPE_STRING: {
            uint64_t length = 0;
            if (!decode_varint(data, end, &length)) {
                return false;
            }
            // Bounds check without pointer overflow
            size_t remaining = (size_t)(end - *data);
            if (length > (uint64_t)remaining) {
                return false;
            }
            *data += (size_t)length;
            break;
        }
        case WIRE_TYPE_32BIT:
            if (*data + 4 > end) {
                return false;
            }
            *data += 4;
            break;
        default:
            return false; // Unknown wire type
    }
    return true;
}
