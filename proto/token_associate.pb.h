/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.5 */

#ifndef PB_HEDERA_PROTO_TOKEN_ASSOCIATE_PB_H_INCLUDED
#define PB_HEDERA_PROTO_TOKEN_ASSOCIATE_PB_H_INCLUDED
#include <pb.h>
#include "proto/basic_types.pb.h"

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
/* *
 Associates the provided account with the provided tokens. Must be signed by
 the provided Account's key. If the provided account is not found, the
 transaction will resolve to INVALID_ACCOUNT_ID. If the provided account has
 been deleted, the transaction will resolve to ACCOUNT_DELETED. If any of the
 provided tokens is not found, the transaction will resolve to
 INVALID_TOKEN_REF. If any of the provided tokens has been deleted, the
 transaction will resolve to TOKEN_WAS_DELETED. If an association between the
 provided account and any of the tokens already exists, the transaction will
 resolve to TOKEN_ALREADY_ASSOCIATED_TO_ACCOUNT. If the provided account's
 associations count exceed the constraint of maximum token associations per
 account, the transaction will resolve to TOKENS_PER_ACCOUNT_LIMIT_EXCEEDED.
 On success, associations between the provided account and tokens are made and
 the account is ready to interact with the tokens. */
typedef struct _Hedera_TokenAssociateTransactionBody { 
    /* *
 The account to be associated with the provided tokens */
    bool has_account;
    Hedera_AccountID account; 
    /* *
 The tokens to be associated with the provided account. In the case of
 NON_FUNGIBLE_UNIQUE Type, once an account is associated, it can hold any
 number of NFTs (serial numbers) of that token type
 Limited to 1 here (no access to malloc for dynamic decode!) */
    pb_size_t tokens_count;
    Hedera_TokenID tokens[1]; 
} Hedera_TokenAssociateTransactionBody;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define Hedera_TokenAssociateTransactionBody_init_default {false, Hedera_AccountID_init_default, 0, {Hedera_TokenID_init_default}}
#define Hedera_TokenAssociateTransactionBody_init_zero {false, Hedera_AccountID_init_zero, 0, {Hedera_TokenID_init_zero}}

/* Field tags (for use in manual encoding/decoding) */
#define Hedera_TokenAssociateTransactionBody_account_tag 1
#define Hedera_TokenAssociateTransactionBody_tokens_tag 2

/* Struct field encoding specification for nanopb */
#define Hedera_TokenAssociateTransactionBody_FIELDLIST(X, a) \
X(a, STATIC,   OPTIONAL, MESSAGE,  account,           1) \
X(a, STATIC,   REPEATED, MESSAGE,  tokens,            2)
#define Hedera_TokenAssociateTransactionBody_CALLBACK NULL
#define Hedera_TokenAssociateTransactionBody_DEFAULT NULL
#define Hedera_TokenAssociateTransactionBody_account_MSGTYPE Hedera_AccountID
#define Hedera_TokenAssociateTransactionBody_tokens_MSGTYPE Hedera_TokenID

extern const pb_msgdesc_t Hedera_TokenAssociateTransactionBody_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define Hedera_TokenAssociateTransactionBody_fields &Hedera_TokenAssociateTransactionBody_msg

/* Maximum encoded size of messages (where known) */
#define Hedera_TokenAssociateTransactionBody_size 94

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
