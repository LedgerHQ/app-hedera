#include "sign_contract_call.h"
#include "printf.h"
#include "app_io.h"

// lightweight helpers to print 64-bit values without %lld support
static void print_int64_field(const char* label, int64_t value) {
    uint32_t hi = (uint32_t)((uint64_t)value >> 32);
    uint32_t lo = (uint32_t)((uint64_t)value & 0xFFFFFFFFu);
    if (hi == 0) {
        PRINTF("%s%u\n", label, (unsigned)lo);
    } else {
        PRINTF("%s0x%08x%08x\n", label, (unsigned)hi, (unsigned)lo);
    }
}

// Contract call handler function
void handle_sign_contract_call(uint8_t p1, uint8_t p2, uint8_t* buffer,
                              uint16_t len,
                              /* out */ volatile unsigned int* flags,
                              /* out */ volatile unsigned int* tx) {
    UNUSED(p1);
    UNUSED(p2);
    UNUSED(tx);

    // Raw contract call data
    uint8_t raw_contract_call[MAX_CONTRACT_CALL_TX_SIZE];

    // Validate input
    if (buffer == NULL || len < 4) {
        THROW(EXCEPTION_MALFORMED_APDU);
    }

    // Validate input length (HERE)
    int raw_contract_call_length = (int)len - 4;
    if (raw_contract_call_length < 0 ||
        raw_contract_call_length > MAX_CONTRACT_CALL_TX_SIZE) {
        THROW(EXCEPTION_MALFORMED_APDU);
    }

    // Key Index (Little Endian format)
    uint32_t key_index = U4LE(buffer, 0);

    // Copy raw contract call data
    memmove(raw_contract_call, (buffer + 4), raw_contract_call_length);

    // Note: Field length validation is enforced before decoding by nanopb.
    // The functionParameters field has a maximum size of 1KB (1024 bytes) as defined
    // in the protobuf schema, preventing oversized function parameters from being processed.

    // Make the in-memory buffer into a stream
    pb_istream_t stream =
        pb_istream_from_buffer(raw_contract_call, raw_contract_call_length);

    // Decode the Contract Call Transaction Body
    Hedera_ContractCallTransactionBody contract_call_tx =
        Hedera_ContractCallTransactionBody_init_zero;
    if (!pb_decode(&stream, Hedera_ContractCallTransactionBody_fields,
                   &contract_call_tx)) {
        PRINTF("Failed to decode contract call protobuf\n");
        THROW(EXCEPTION_MALFORMED_APDU);
    }

    // Print all contract call fields
    PRINTF("=== Contract Call Transaction ===\n");
    PRINTF("Key Index: %u\n", key_index);
    
    // Contract ID
    if (contract_call_tx.has_contractID) {
        PRINTF("Contract ID:\n");
        print_int64_field("  Shard: ", contract_call_tx.contractID.shardNum);
        print_int64_field("  Realm: ", contract_call_tx.contractID.realmNum);
        PRINTF("  Which Contract: %u\n",
               (unsigned)contract_call_tx.contractID.which_contract);
        
        if (contract_call_tx.contractID.which_contract ==
            Hedera_ContractID_contractNum_tag) {
            print_int64_field("  Contract Number: ",
                               contract_call_tx.contractID.contract.contractNum);
        } else if (contract_call_tx.contractID.which_contract ==
                   Hedera_ContractID_evm_address_tag) {
            PRINTF("  EVM Address: ");
            for (size_t i = 0;
                 i < contract_call_tx.contractID.contract.evm_address.size;
                 i++) {
                PRINTF("%02x",
                       contract_call_tx.contractID.contract.evm_address.bytes[i]);
            }
            PRINTF("\n");
        }
    } else {
        PRINTF("Contract ID: Not set\n");
    }
    
    // Gas limit
    print_int64_field("Gas Limit: ", contract_call_tx.gas);
    
    // Amount (tinybar)
    print_int64_field("Amount (tinybar): ", contract_call_tx.amount);
    
    // Function parameters
    if (contract_call_tx.functionParameters.size > 0) {
        PRINTF("Function Parameters Length: %u\n",
               (unsigned)contract_call_tx.functionParameters.size);
        PRINTF("Function Parameters (hex): ");
        for (size_t i = 0; i < contract_call_tx.functionParameters.size; i++) {
            PRINTF("%02x", contract_call_tx.functionParameters.bytes[i]);
        }
        PRINTF("\n");
    } else {
        PRINTF("Function Parameters: Not set\n");
    }
    
    PRINTF("=== End Contract Call ===\n");

    // For now, just return success without actual signing
    // TODO: Implement actual contract call signing logic
    io_exchange_with_code(0x9000, 0);
    *flags |= IO_ASYNCH_REPLY;
}
