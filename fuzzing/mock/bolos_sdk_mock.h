#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

// Define NO_BOLOS_SDK to disable actual SDK includes
#define NO_BOLOS_SDK 1

// Mock types for BOLOS SDK
typedef uint32_t cx_err_t;
typedef uint32_t cx_curve_t;
typedef uint32_t cx_md_t;

#define CX_OK 0x9000

// Mock cryptographic structures
typedef struct {
    uint8_t data[64];
} cx_hash_t;

typedef struct {
    uint8_t data[32];
} cx_sha3_t;

typedef struct {
    uint8_t data[65];
} cx_ecfp_public_key_t;

typedef struct {
    uint8_t data[32];
} cx_ecfp_private_key_t;

// Mock UI structures
typedef struct {
    uint32_t component;
    uint32_t userid;
} bagl_element_t;

typedef enum {
    TYPE_TRANSACTION,
    TYPE_MESSAGE
} nbgl_operationType_t;

typedef struct {
    const char** pairs;
    uint8_t nbPairs;
} nbgl_contentTagValueList_t;

typedef struct {
    const uint8_t* bitmap;
    uint32_t width;
    uint32_t height;
} nbgl_icon_details_t;

typedef void (*nbgl_choiceCallback_t)(bool confirm);

// Exception codes
#define EXCEPTION_IO_RESET          0x6700
#define EXCEPTION_OVERFLOW          0x6701
#define EXCEPTION_SECURITY          0x6702
#define EXCEPTION_INVALID_PARAMETER 0x6B00
#define EXCEPTION_MALFORMED_APDU    0x6E00

// Mock function declarations
void PRINTF(const char* format, ...);
void explicit_bzero(void *s, size_t n);
void* pic(void* ptr);

// Cryptographic functions
cx_err_t cx_keccak_init_no_throw(cx_sha3_t *hash, size_t size);
cx_err_t cx_hash_no_throw(cx_hash_t *hash, uint32_t mode, const uint8_t *in, size_t len, uint8_t *out, size_t out_len);
cx_err_t cx_ecdsa_sign_no_throw(const cx_ecfp_private_key_t *key,
                                uint32_t mode,
                                cx_md_t hashID,
                                const uint8_t *hash,
                                size_t hash_len,
                                uint8_t *sig,
                                size_t *sig_len,
                                uint32_t *info);
cx_err_t cx_ecfp_generate_pair_no_throw(cx_curve_t curve,
                                        cx_ecfp_public_key_t *pubkey,
                                        cx_ecfp_private_key_t *privkey,
                                        bool keepprivate);

// UI functions
void ui_idle(void);
unsigned int ui_approval_nanos_review(const bagl_element_t* elements,
                                     unsigned int elements_count,
                                     unsigned int button_mask);
void nbgl_useCaseReview(nbgl_operationType_t operationType,
                       const nbgl_contentTagValueList_t* tagValueList,
                       const nbgl_icon_details_t* icon,
                       const char* reviewTitle,
                       const char* reviewSubTitle,
                       const char* finishTitle,
                       nbgl_choiceCallback_t choiceCallback);

// Error handling
void THROW(unsigned int exception);

// Memory management
void* os_malloc(size_t size);
void os_free(void* ptr);
