#include "sign_contract_call.h"

#include <string.h>

#include "app_io.h"
#include "evm_parser.h"
#include "hedera_format.h"
#include "printf.h"
#include "sign_transaction.h"
#include "tokens/token_address.h"

// // lightweight helpers to print 64-bit values without %lld support
// static void print_int64_field(const char* label, int64_t value) {
//     uint32_t hi = (uint32_t)((uint64_t)value >> 32);
//     uint32_t lo = (uint32_t)((uint64_t)value & 0xFFFFFFFFu);
//     if (hi == 0) {
//         PRINTF("%s%u\n", label, (unsigned)lo);
//     } else {
//         PRINTF("%s0x%08x%08x\n", label, (unsigned)hi, (unsigned)lo);
//     }
// }

bool validate_and_reformat_contract_call(
    Hedera_ContractCallTransactionBody* contract_call_tx) {
    if (contract_call_tx == NULL) {
        PRINTF("Contract call transaction is NULL\n");
        return false;
    }
    
    if (contract_call_tx->functionParameters.size < EVM_SELECTOR_SIZE) {
        PRINTF("Function parameters too short for selector\n");
        return false;
    }
    
    uint32_t function_selector =
        U4BE(contract_call_tx->functionParameters.bytes, 0);

    switch (function_selector) {
        case EVM_ERC20_TRANSFER_SELECTOR: {
            // Parse ERC-20 transfer(address,uint256) parameters
            transfer_calldata_t transfer_data;
            if (parse_transfer_function(
                    contract_call_tx->functionParameters.bytes,
                    contract_call_tx->functionParameters.size,
                    &transfer_data)) {
                if (!evm_addr_to_str(&transfer_data.to, st_ctx.recipients)) {
                    PRINTF("Failed to stringify EVM address\n");
                    return false;
                }

                if (!evm_word_to_amount(transfer_data.amount.bytes,
                                        st_ctx.amount)) {
                    PRINTF("Failed to stringify amount word\n");
                    return false;
                }

            } else {
                PRINTF("Failed to parse ERC-20 transfer parameters\n");
                return false;
            }

            // Print contract ID
            if (contract_call_tx->contractID.which_contract ==
                Hedera_ContractID_contractNum_tag) {
                token_addr_t contract_id = {
                    contract_call_tx->contractID.shardNum,
                    contract_call_tx->contractID.realmNum,
                    contract_call_tx->contractID.contract.contractNum,
                };
                // 0.0.XXXX format for contract ID
                address_to_string(&contract_id, st_ctx.senders);
            } else if (contract_call_tx->contractID.which_contract ==
                       Hedera_ContractID_evm_address_tag) {
                // 0xXXXX format for EVM address
                if (contract_call_tx->contractID.contract.evm_address.size != EVM_ADDRESS_SIZE) {
                    PRINTF("Invalid EVM address size: %d\n", 
                           contract_call_tx->contractID.contract.evm_address.size);
                    return false;
                }
                evm_address_t evm_address;
                // Copy the 20-byte EVM address from protobuf bytes array
                memcpy(evm_address.bytes,
                       contract_call_tx->contractID.contract.evm_address.bytes,
                       EVM_ADDRESS_SIZE);
                char evm_addr_str[EVM_ADDRESS_STR_SIZE];
                if (!evm_addr_to_str(&evm_address, evm_addr_str)) {
                    PRINTF("Failed to stringify EVM address\n");
                    return false;
                }
                hedera_safe_printf(st_ctx.senders, "%s", evm_addr_str);
            } else {
                PRINTF("Unsupported contract ID type: %d\n", 
                       contract_call_tx->contractID.which_contract);
                return false;
            }
            // Validate and print gas
            if (contract_call_tx->gas <= 0) {
                PRINTF("Invalid gas value: %lld\n", contract_call_tx->gas);
                return false;
            }
            hedera_safe_printf(st_ctx.auto_renew_period, "%llu",
                               contract_call_tx->gas);
            
            // Validate and print amount in tinybar
            if (contract_call_tx->amount < 0) {
                PRINTF("Invalid amount value: %lld\n", contract_call_tx->amount);
                return false;
            }
            hedera_safe_printf(st_ctx.expiration_time, "%llu hbar",
                               contract_call_tx->amount);
            break;
        }
        // To future developers: Add more supported function selectors here :D
        default:
            PRINTF("Unsupported function selector: %x\n", function_selector);
            return false;
    }

    PRINTF("==========================================\n");
    PRINTF("Formatted fields\n");
    PRINTF("Key Index: %s\n", st_ctx.key_index_str);
    PRINTF("From: %s\n", st_ctx.operator);
    PRINTF("To: %s\n", st_ctx.recipients);
    PRINTF("Contract: %s\n", st_ctx.senders);
    PRINTF("Gas limit: %s\n", st_ctx.auto_renew_period);
    PRINTF("Raw token amount: %s\n", st_ctx.amount);
    PRINTF("HBAR sent: %s\n", st_ctx.expiration_time);
    PRINTF("Fee: %s\n", st_ctx.fee);
    PRINTF("==========================================\n");
    return true;
}

// Contract call handler function
void handle_contract_call_body() {
    // Decode the Contract Call Transaction Body
    Hedera_ContractCallTransactionBody contract_call_tx =
        st_ctx.transaction.data.contractCall;

    // Verify fields and extract fields to UI global context
    if (!validate_and_reformat_contract_call(&contract_call_tx)) {
        THROW(EXCEPTION_MALFORMED_APDU);
    }
}