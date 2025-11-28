#include "bolos_sdk_mock.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

// Mock PRINTF for fuzzing
void PRINTF(const char* format, ...) {
    // Suppress output during fuzzing for performance
    (void)format;
}

// Mock explicit_bzero
void explicit_bzero(void *s, size_t n) {
    memset(s, 0, n);
}

// Mock pic function
void* pic(void* ptr) {
    return ptr;
}

// Mock CX functions for cryptographic operations
cx_err_t cx_keccak_init_no_throw(cx_sha3_t *hash, size_t size) {
    (void)hash;
    (void)size;
    return CX_OK;
}

cx_err_t cx_hash_no_throw(cx_hash_t *hash, uint32_t mode, const uint8_t *in, size_t len, uint8_t *out, size_t out_len) {
    (void)hash;
    (void)mode;
    (void)in;
    (void)len;
    
    // Simple mock - fill output with predictable pattern
    if (out && out_len > 0) {
        memset(out, 0xAB, out_len);
    }
    return CX_OK;
}

cx_err_t cx_ecdsa_sign_no_throw(const cx_ecfp_private_key_t *key,
                                uint32_t mode,
                                cx_md_t hashID,
                                const uint8_t *hash,
                                size_t hash_len,
                                uint8_t *sig,
                                size_t *sig_len,
                                uint32_t *info) {
    (void)key;
    (void)mode;
    (void)hashID;
    (void)hash;
    (void)hash_len;
    (void)info;
    
    // Mock signature - fill with predictable pattern
    if (sig && sig_len && *sig_len >= 64) {
        memset(sig, 0xCD, 64);
        *sig_len = 64;
    }
    return CX_OK;
}

// Mock BIP32 functions
cx_err_t cx_ecfp_generate_pair_no_throw(cx_curve_t curve,
                                        cx_ecfp_public_key_t *pubkey,
                                        cx_ecfp_private_key_t *privkey,
                                        bool keepprivate) {
    (void)curve;
    (void)keepprivate;
    
    // Mock key generation
    if (pubkey) {
        memset(pubkey, 0xEF, sizeof(cx_ecfp_public_key_t));
    }
    if (privkey) {
        memset(privkey, 0x12, sizeof(cx_ecfp_private_key_t));
    }
    return CX_OK;
}

// Mock UI functions
void ui_idle(void) {
    // No-op during fuzzing
}

unsigned int ui_approval_nanos_review(const bagl_element_t* elements,
                                     unsigned int elements_count,
                                     unsigned int button_mask) {
    (void)elements;
    (void)elements_count;
    (void)button_mask;
    return 0;
}

// Mock NBGL functions for newer devices
void nbgl_useCaseReview(nbgl_operationType_t operationType,
                       const nbgl_contentTagValueList_t* tagValueList,
                       const nbgl_icon_details_t* icon,
                       const char* reviewTitle,
                       const char* reviewSubTitle,
                       const char* finishTitle,
                       nbgl_choiceCallback_t choiceCallback) {
    (void)operationType;
    (void)tagValueList;
    (void)icon;
    (void)reviewTitle;
    (void)reviewSubTitle;
    (void)finishTitle;
    
    // Simulate approval for fuzzing
    if (choiceCallback) {
        choiceCallback(true);
    }
}

// Unified last-throw symbol (same as unit tests)
volatile unsigned int g_last_throw = 0;

// Mock memory management
void* os_malloc(size_t size) {
    return malloc(size);
}

void os_free(void* ptr) {
    free(ptr);
}
