#ifndef __TOKEN_ADDRESS_H__
#define __TOKEN_ADDRESS_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//Typedef for the token address in the format of (u64,u64,u64)
typedef struct token_addr {
    uint64_t addr_account;
    uint64_t addr_realm;
    uint64_t addr_shard;
} token_addr_t;
#endif // __TOKEN_ADDRESS_H__