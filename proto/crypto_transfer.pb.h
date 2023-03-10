/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.5 */

#ifndef PB_HEDERA_PROTO_CRYPTO_TRANSFER_PB_H_INCLUDED
#define PB_HEDERA_PROTO_CRYPTO_TRANSFER_PB_H_INCLUDED
#include <pb.h>
#include "proto/basic_types.pb.h"

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
/* *
 Transfers cryptocurrency among two or more accounts by making the desired
 adjustments to their balances. Each transfer list can specify up to 10
 adjustments. Each negative amount is withdrawn from the corresponding
 account (a sender), and each positive one is added to the corresponding
 account (a receiver). The amounts list must sum to zero. Each amount is a
 number of tinybars (there are 100,000,000 tinybars in one hbar).  If any
 sender account fails to have sufficient hbars, then the entire transaction
 fails, and none of those transfers occur, though the transaction fee is
 still charged. This transaction must be signed by the keys for all the
 sending accounts, and for any receiving accounts that have
 receiverSigRequired == true. The signatures are in the same order as the
 accounts, skipping those accounts that don't need a signature. */
typedef struct _Hedera_CryptoTransferTransactionBody { 
    /* *
 The desired hbar balance adjustments */
    bool has_transfers;
    Hedera_TransferList transfers; 
    /* *
 The desired token unit balance adjustments; if any custom fees are
 assessed, the ledger will try to deduct them from the payer of this
 CryptoTransfer, resolving the transaction to
 INSUFFICIENT_PAYER_BALANCE_FOR_CUSTOM_FEE if this is not possible
 Limited to 1 here */
    pb_size_t tokenTransfers_count;
    Hedera_TokenTransferList tokenTransfers[1]; 
} Hedera_CryptoTransferTransactionBody;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define Hedera_CryptoTransferTransactionBody_init_default {false, Hedera_TransferList_init_default, 0, {Hedera_TokenTransferList_init_default}}
#define Hedera_CryptoTransferTransactionBody_init_zero {false, Hedera_TransferList_init_zero, 0, {Hedera_TokenTransferList_init_zero}}

/* Field tags (for use in manual encoding/decoding) */
#define Hedera_CryptoTransferTransactionBody_transfers_tag 1
#define Hedera_CryptoTransferTransactionBody_tokenTransfers_tag 2

/* Struct field encoding specification for nanopb */
#define Hedera_CryptoTransferTransactionBody_FIELDLIST(X, a) \
X(a, STATIC,   OPTIONAL, MESSAGE,  transfers,         1) \
X(a, STATIC,   REPEATED, MESSAGE,  tokenTransfers,    2)
#define Hedera_CryptoTransferTransactionBody_CALLBACK NULL
#define Hedera_CryptoTransferTransactionBody_DEFAULT NULL
#define Hedera_CryptoTransferTransactionBody_transfers_MSGTYPE Hedera_TransferList
#define Hedera_CryptoTransferTransactionBody_tokenTransfers_MSGTYPE Hedera_TokenTransferList

extern const pb_msgdesc_t Hedera_CryptoTransferTransactionBody_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define Hedera_CryptoTransferTransactionBody_fields &Hedera_CryptoTransferTransactionBody_msg

/* Maximum encoded size of messages (where known) */
#define Hedera_CryptoTransferTransactionBody_size 475

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif