#include "sign_contract_call.h"

#include <string.h>

// Platform headers not needed directly here; pulled through other includes

#include "evm_parser.h"
#include "ui/app_globals.h"
#include "hedera_format.h"
#include "printf.h"
#include "sign_transaction.h"
#include "tokens/token_address.h"
#include "tokens/cal/token_lookup.h"

// Handle ERC-20 transfer function call
static bool handle_erc20_transfer_call(Hedera_ContractCallTransactionBody* contract_call_tx) {
    // Parse ERC-20 transfer(address,uint256) parameters
    transfer_calldata_t transfer_data;
    if (parse_transfer_function(
            contract_call_tx->functionParameters.bytes,
            contract_call_tx->functionParameters.size,
            &transfer_data)) {
        if (!evm_addr_to_str(&transfer_data.to, st_ctx.recipients, sizeof(st_ctx.recipients))) {
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

    // Print contract ID and try to resolve token metadata (ticker/decimals)
    if (contract_call_tx->contractID.which_contract ==
        Hedera_ContractID_contractNum_tag) {
        // Reject negative contract identifiers
        if (contract_call_tx->contractID.shardNum < 0 ||
            contract_call_tx->contractID.realmNum < 0 ||
            contract_call_tx->contractID.contract.contractNum < 0) {
            PRINTF("Negative contract ID parts not allowed\n");
            return false;
        }
        token_addr_t contract_id = {
            contract_call_tx->contractID.shardNum,
            contract_call_tx->contractID.realmNum,
            contract_call_tx->contractID.contract.contractNum,
        };
        // 0.0.XXXX format for contract ID
        address_to_string(&contract_id, st_ctx.senders);

        // Lookup by Hedera token ID if this contractNum is an HTS token
        st_ctx.token_known = token_info_get_by_address(
            contract_id, st_ctx.token_ticker, st_ctx.token_name, &st_ctx.token_decimals);
    } else if (contract_call_tx->contractID.which_contract ==
               Hedera_ContractID_evm_address_tag) {
        // 0xXXXX format for EVM address
        if (contract_call_tx->contractID.contract.evm_address.size != EVM_ADDRESS_SIZE) {
            PRINTF("Invalid EVM address size: %u\n",
                   (unsigned)contract_call_tx->contractID.contract.evm_address.size);
            return false;
        }
        evm_address_t evm_address;
        // Copy the 20-byte EVM address from protobuf bytes array
        memcpy(evm_address.bytes,
               contract_call_tx->contractID.contract.evm_address.bytes,
               EVM_ADDRESS_SIZE);
        if (!evm_addr_to_str(&evm_address, st_ctx.senders, sizeof(st_ctx.senders))) {
            PRINTF("Failed to stringify EVM address\n");
            return false;
        }

        // Lookup by EVM address
        st_ctx.token_known = token_info_get_by_evm_address(
            &evm_address, st_ctx.token_ticker, st_ctx.token_name, &st_ctx.token_decimals);
    } else {
        PRINTF("Unsupported contract ID type: %u\n",
               (unsigned)contract_call_tx->contractID.which_contract);
        return false;
    }
    
    // If token is known, format amount with decimals and ticker and set UI label
    if (st_ctx.token_known) {
        char formatted[MAX_UINT256_LENGTH + 2] = {0};
        if (!evm_amount_to_string(transfer_data.amount.bytes,
                                  EVM_WORD_SIZE,
                                  (uint8_t)st_ctx.token_decimals,
                                  st_ctx.token_ticker,
                                  formatted,
                                  sizeof(formatted))) {
            PRINTF("Failed to format ERC20 amount with evm_amount_to_string\n");
            return false;
        }
        hedera_safe_printf(st_ctx.amount, "%s", formatted);
        hedera_safe_printf(st_ctx.amount_title, "%s", "Token amount");
    } else {
        hedera_safe_printf(st_ctx.amount_title, "%s", "Raw token amount");
    }

    // Validate and print gas - Gas is int64 in upstream proto; app rejects negatives
    if (contract_call_tx->gas < 0) {
        PRINTF("Invalid gas value: %lld\n", (long long)contract_call_tx->gas);
        return false;
    }
    hedera_safe_printf(st_ctx.auto_renew_period, "%llu",
                       (unsigned long long)contract_call_tx->gas);
    
    // Validate and print amount in HBAR (tinybar -> HBAR with decimals)
    if (contract_call_tx->amount < 0) {
        PRINTF("Invalid amount value: %lld\n", (long long)contract_call_tx->amount);
        return false;
    }
    hedera_safe_printf(st_ctx.expiration_time, "%s hbar",
                       hedera_format_tinybar_str((uint64_t)contract_call_tx->amount));

    // Format ERC20 amount using decimals if known
    if (st_ctx.token_known) {
        char formatted[MAX_UINT256_LENGTH + 2] = {0};
        if (!evm_amount_to_string(transfer_data.amount.bytes,
                            EVM_WORD_SIZE,
                            (uint8_t)st_ctx.token_decimals,
                            st_ctx.token_ticker,
                            formatted,
                            sizeof(formatted))) {
            PRINTF("Failed to format ERC20 amount with evm_amount_to_string\n");
            return false;
        }
        hedera_safe_printf(st_ctx.amount, "%s", formatted);
    }
    
    return true;
}

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
    // Guard against oversized calldata
    if (contract_call_tx->functionParameters.size > MAX_CONTRACT_CALL_TX_SIZE) {
        PRINTF("Function parameters too large: %u\n",
               (unsigned)contract_call_tx->functionParameters.size);
        return false;
    }
    
    uint32_t function_selector =
        U4BE(contract_call_tx->functionParameters.bytes, 0);

    switch (function_selector) {
        case EVM_ERC20_TRANSFER_SELECTOR:
            // ERC-20 transfer must be exactly 4 + 32 + 32 bytes
            if (contract_call_tx->functionParameters.size !=
                (EVM_SELECTOR_SIZE + 2 * EVM_WORD_SIZE)) {
                PRINTF("Invalid ERC-20 transfer params length: %u\n",
                       (unsigned)contract_call_tx->functionParameters.size);
                return false;
            }
            if (!handle_erc20_transfer_call(contract_call_tx)) {
                return false;
            }
            break;
        // To future developers: Add more supported function selectors here :D
        default:
            PRINTF("Unsupported function selector: %x\n", function_selector);
            return false;
    }
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