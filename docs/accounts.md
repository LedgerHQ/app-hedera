# Hedera Accounts
Hedera accounts differ from those on other blockchains.
They do not use addresses derived from public keys.
Instead, Hedera accounts use a triplet `(shard, realm, num)` to identify an account.
Therefore, we cannot directly verify an account ID with a public key.

## Derivation Path
Compared to other blockchains, Hedera wallets often use a simplified derivation path, typically set to 
`m/44'/3030'/0'/0'/0'`, where index `0` represents the account identifier. Creating multiple accounts from a single key pair is common practice. For example, HashPack (a reference wallet mentioned here: https://support.ledger.com/article/4494505217565-zd) creates multiple accounts from a single seed using the same derivation path. 
Each account has an assigned admin key, which is the public key of that account.

### Derivation Path in Device App - Transaction Signing and Public Key Operations
For transaction signing (`handle_sign_transaction`) and public key operations (`handle_get_public_key`), the Hedera device app uses a simplified approach:

**Hardcoded Path Structure** (defined in `app_globals.h`):
- `PATH_ZERO`: `44'` (BIP44 purpose)
- `PATH_ONE`: `3030'` (Hedera HBAR coin type)
- `PATH_TWO`: `0'` (hardened account level)
- `PATH_THREE`: `0'` (hardened change level)  
- `PATH_FOUR`: `index'` (hardened address index)

**Implementation Details:**
- Both operations accept only a single `uint32_t index` parameter
- **Format**: Index is expected in **Little Endian** format (4 bytes)
- The `hedera_set_path()` function in `hedera.c` constructs the full path: `m/44'/3030'/0'/0'/index'`
- Both `hedera_get_pubkey()` and `hedera_sign()` use this fixed structure
- The Makefile restricts allowed derivation paths to `"44'/3030'"` for security

**Code Flow:**
```c
// Transaction signing: handle_sign_transaction()
st_ctx.key_index = U4LE(buffer, 0);  // Extract index from APDU (Little Endian)
hedera_sign(st_ctx.key_index, ...)   // Sign with hardcoded path

// Public key: handle_get_public_key() 
gpk_ctx.key_index = U4LE(buffer, 0); // Extract index from APDU (Little Endian)
hedera_get_pubkey(gpk_ctx.key_index, ...) // Derive key with hardcoded path
```

### Derivation Path in Device App - Swap Implementation
The swap functionality (`handle_check_address.c`) implements flexible derivation path handling to support various wallet implementations:

**Supported Path Formats:**
1. **Full derivation path**: Complete paths like `m/44'/3030'/0'/0'/index'` (21 bytes when serialized)
2. **Shortened paths**: Paths containing only the account index (5 bytes when serialized)
3. **Current Ledger Live format**: Paths with `m/44'/3030'` without index (9 bytes when serialized)

**Flexible Path Processing Logic:**
```c
// Validate path length and alignment
if (params->address_parameters_length < 5 || params->address_parameters_length > 21 || 
    (params->address_parameters_length-1) % 4 != 0) {
    PRINTF("Invalid deriv path length: %d\n", params->address_parameters_length);
    return 0;
}

// Extract index from last 4 bytes (Big Endian format)
uint32_t index = U4BE(params->address_parameters, params->address_parameters_length - 4);

// Handle current Ledger Live hederaBip44 derivation mode
// Where Ledger Live sends m/44'/3030' path without index, so we assume index=0
if (params->address_parameters_length == 9) {
    index = 0;
}
```

**Format Differences:**
- **Endianness**: Swap operations expect derivation path components in **Big Endian** format
- **Structure**: Variable-length serialized paths (5-21 bytes) with 4-byte alignment
- **Parsing**: Index extracted from the last 4 bytes of the path buffer

**Wallet Compatibility:**
- **Ledger Live**: Uses `hederaBip44` derivation mode with `"44'/3030'"` path, assumes `index=0` (defined in `ledger-live/libs/coin-framework/src/derivation.ts`)
- **Third-party wallets**: Can send various path formats (longer, shorter, or index-only) to support different account indices
- **Validation**: Ensures path lengths are within acceptable ranges (5-21 bytes) with proper 4-byte alignment for security

### Derivation Path in Ledger Live
Ledger Live uses the `hw-app-hedera` package to communicate with the device app. The implementation accepts BIP-32 formatted path strings (e.g., `"m/44'/3030'/0'/0'/0'"`) through the `getPublicKey(path: string)` method in `Hedera.ts`.

The path is processed as follows:
1. Converted from string format to a BIP-32 path array using `BIPPath.fromString(path).toPathArray()`
2. Serialized into a buffer format compatible with the device app via `_serializePath()`
3. Sent to the device app via APDU commands

Currently, the device app primarily supports index `#0` for transaction signing, though it can derive public keys for other indices.

## Swaps 
During the swapping process, we need to verify destination and refund addresses.
Since we cannot directly verify an account ID with a public key, `app-exchange` must use the Trusted Service to fetch the public key related to an account ID in a signed payload.

### Trusted Service
To address this issue, Hedera uses a signed endpoint called the **Trusted Service**.
This service is used to fetch the public key related to an account ID in a signed payload.

The endpoint is: `https://nft.api.live.ledger.com/v2/hedera/pubkey/{account_triplet}?challenge={challenge_from_app_exchange}`.

The service returns a response containing:
- `account`: The account ID (shard.realm.num format)
- `key`: The public key associated with the account
- `signedDescriptor`: A cryptographic signature validating the relationship

The Trusted Service is transparent to the Hedera app; all verification and account ID to key mapping for swaps is handled on the `app-exchange` side.

This trusted service is essential for swap operations because Hedera's account system does not allow direct derivation of account IDs from public keys, unlike most other blockchain networks.