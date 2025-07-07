#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * Protobuf wire types for second-stage decoding
 */
typedef enum {
    WIRE_TYPE_VARINT = 0,
    WIRE_TYPE_64BIT = 1,
    WIRE_TYPE_STRING = 2,
    WIRE_TYPE_START_GROUP = 3,  // deprecated
    WIRE_TYPE_END_GROUP = 4,    // deprecated
    WIRE_TYPE_32BIT = 5
} wire_type_t;

/**
 * Protobuf field structure for second-stage parsing
 */
typedef struct {
    uint32_t field_number;
    wire_type_t wire_type;
    const uint8_t *data;
    size_t length;
} protobuf_field_t;

/**
 * Second-stage protobuf decoder for nested StringValue fields
 * 
 * This function performs targeted extraction of StringValue fields from
 * protobuf messages that have already been partially decoded by nanopb.
 * It handles the nested structure: TransactionBody -> CryptoUpdateTransactionBody -> StringValue
 * 
 * @param buffer Raw protobuf data from the transaction
 * @param buffer_size Length of the buffer
 * @param field_number Target field number to extract (e.g., 14 for memo)
 * @param output Buffer to store the extracted string
 * @param output_size Size of the output buffer
 * @return true if field was found and extracted successfully, false otherwise
 */
bool extract_nested_string_field(const uint8_t *buffer, size_t buffer_size, 
                                  uint32_t field_number, char *output, size_t output_size);

/**
 * Parse a protobuf field tag and wire type for second-stage decoding
 * 
 * @param data Pointer to the data (will be advanced)
 * @param end Pointer to the end of data
 * @param field Pointer to store the parsed field info
 * @return true if successful, false if malformed
 */
bool parse_field_tag(const uint8_t **data, const uint8_t *end, protobuf_field_t *field);
