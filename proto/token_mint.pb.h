/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.5 */

#ifndef PB_HEDERA_PROTO_TOKEN_MINT_PB_H_INCLUDED
#define PB_HEDERA_PROTO_TOKEN_MINT_PB_H_INCLUDED
#include <pb.h>
#include "proto/basic_types.pb.h"

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef PB_BYTES_ARRAY_T(100) Hedera_TokenMintTransactionBody_metadata_t;
/* *
 Mints tokens to the Token's treasury Account. If no Supply Key is defined,
 the transaction will resolve to TOKEN_HAS_NO_SUPPLY_KEY. The operation
 increases the Total Supply of the Token. The maximum total supply a token can
 have is 2^63-1. The amount provided must be in the lowest denomination
 possible. Example: Token A has 2 decimals. In order to mint 100 tokens, one
 must provide amount of 10000. In order to mint 100.55 tokens, one must
 provide amount of 10055. If both amount and metadata list get filled, a
 INVALID_TRANSACTION_BODY response code will be returned. If the metadata list
 contains metadata which is too large, a METADATA_TOO_LONG response code will
 be returned.
 If neither the amount nor the metadata list get filled, a
 INVALID_TOKEN_MINT_AMOUNT response code will be returned. If the metadata
 list count is greater than the batch size limit global dynamic property, a
 BATCH_SIZE_LIMIT_EXCEEDED response code will be returned. */
typedef struct _Hedera_TokenMintTransactionBody { 
    /* *
 The token for which to mint tokens. If token does not exist, transaction
 results in INVALID_TOKEN_ID */
    bool has_token;
    Hedera_TokenID token; 
    /* *
 Applicable to tokens of type FUNGIBLE_COMMON. The amount to mint to the
 Treasury Account. Amount must be a positive non-zero number represented in
 the lowest denomination of the token. The new supply must be lower than
 2^63. */
    uint64_t amount; 
    /* *
 Applicable to tokens of type NON_FUNGIBLE_UNIQUE. A list of metadata that
 are being created. Maximum allowed size of each metadata is 100 bytes
 Limited to 1 metadata chunk (no access to malloc) */
    pb_size_t metadata_count;
    Hedera_TokenMintTransactionBody_metadata_t metadata[1]; 
} Hedera_TokenMintTransactionBody;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define Hedera_TokenMintTransactionBody_init_default {false, Hedera_TokenID_init_default, 0, 0, {{0, {0}}}}
#define Hedera_TokenMintTransactionBody_init_zero {false, Hedera_TokenID_init_zero, 0, 0, {{0, {0}}}}

/* Field tags (for use in manual encoding/decoding) */
#define Hedera_TokenMintTransactionBody_token_tag 1
#define Hedera_TokenMintTransactionBody_amount_tag 2
#define Hedera_TokenMintTransactionBody_metadata_tag 3

/* Struct field encoding specification for nanopb */
#define Hedera_TokenMintTransactionBody_FIELDLIST(X, a) \
X(a, STATIC,   OPTIONAL, MESSAGE,  token,             1) \
X(a, STATIC,   SINGULAR, UINT64,   amount,            2) \
X(a, STATIC,   REPEATED, BYTES,    metadata,          3)
#define Hedera_TokenMintTransactionBody_CALLBACK NULL
#define Hedera_TokenMintTransactionBody_DEFAULT NULL
#define Hedera_TokenMintTransactionBody_token_MSGTYPE Hedera_TokenID

extern const pb_msgdesc_t Hedera_TokenMintTransactionBody_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define Hedera_TokenMintTransactionBody_fields &Hedera_TokenMintTransactionBody_msg

/* Maximum encoded size of messages (where known) */
#define Hedera_TokenMintTransactionBody_size     149

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
