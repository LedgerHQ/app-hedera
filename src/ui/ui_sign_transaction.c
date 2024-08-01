#include "glyphs.h"
#include "proto/crypto_create.pb.h"
#include "sign_transaction.h"
#include "ui_common.h"
#include "ux.h"

#ifdef HAVE_NBGL
#include "nbgl_page.h"
#include "nbgl_use_case.h"
#endif

#if defined(TARGET_NANOS)

static uint8_t num_screens(size_t length) {
    // Number of screens is len / display size + 1 for overflow
    if (length == 0) return 1;

    uint8_t screens = length / DISPLAY_SIZE;

    if (length % DISPLAY_SIZE > 0) {
        screens += 1;
    }

    return screens;
}

bool first_screen() { return st_ctx.display_index == 1; }

bool last_screen() { return st_ctx.display_index == st_ctx.display_count; }

static void update_display_count(void) {
    st_ctx.display_count = num_screens(strlen(st_ctx.full));
}

static void shift_display(void) {
    // Slide window (partial) along full entity (full) by DISPLAY_SIZE chars
    MEMCLEAR(st_ctx.partial);
    memmove(st_ctx.partial,
            st_ctx.full + (DISPLAY_SIZE * (st_ctx.display_index - 1)),
            DISPLAY_SIZE);
}

static void reformat_senders(void) {
    switch (st_ctx.type) {
        case Verify:
            reformat_verify_account();
            break;

        case Create:
        case Update:
            reformat_stake_target();
            break;

        case Associate:
            reformat_token_associate();
            break;

        case Dissociate:
            reformat_token_dissociate();
            break;

        case TokenMint:
            reformat_token_mint();
            break;

        case TokenBurn:
            reformat_token_burn();
            break;

        case TokenTransfer:
            reformat_token_sender_account();
            break;

        case Transfer:
            reformat_sender_account();
            break;

        default:
            return;
    }
}

static void reformat_recipients(void) {
    switch (st_ctx.type) {
        case Create:
        case Update:
            reformat_collect_rewards();
            break;

        case TokenTransfer:
            reformat_token_recipient_account();
            break;

        case Transfer:
            reformat_recipient_account();
            break;

        default:
            return;
    }
}

static void reformat_amount(void) {
    switch (st_ctx.type) {
        case Create:
            reformat_amount_balance();
            break;

        case Update:
            reformat_updated_account();
            break;

        case Transfer:
            reformat_amount_transfer();
            break;

        case TokenMint:
            reformat_amount_mint();
            break;

        case TokenBurn:
            reformat_amount_burn();
            break;

        case TokenTransfer:
            reformat_token_transfer();
            break;

        default:
            return;
    }
}

// Forward declarations for Nano S UI
// Step 1
unsigned int ui_tx_summary_step_button(unsigned int button_mask,
                                       unsigned int button_mask_counter);

// Step 2 - 7
void handle_intermediate_left_press();
void handle_intermediate_right_press();
unsigned int ui_tx_intermediate_step_button(unsigned int button_mask,
                                            unsigned int button_mask_counter);

// Step 8
unsigned int ui_tx_confirm_step_button(unsigned int button_mask,
                                       unsigned int button_mask_counter);

// Step 9
unsigned int ui_tx_deny_step_button(unsigned int button_mask,
                                    unsigned int button_mask_counter);

// UI Definition for Nano S
// Step 1: Transaction Summary
static const bagl_element_t ui_tx_summary_step[] = {
    UI_BACKGROUND(), UI_ICON_RIGHT(RIGHT_ICON_ID, BAGL_GLYPH_ICON_RIGHT),

    // ()       >>
    // Line 1
    // Line 2

    UI_TEXT(LINE_1_ID, 0, 12, 128, st_ctx.summary_line_1),
    UI_TEXT(LINE_2_ID, 0, 26, 128, st_ctx.summary_line_2)};

// Step 2 - 7: Operator, Senders, Recipients, Amount, Fee, Memo
static const bagl_element_t ui_tx_intermediate_step[] = {
    UI_BACKGROUND(), UI_ICON_LEFT(LEFT_ICON_ID, BAGL_GLYPH_ICON_LEFT),
    UI_ICON_RIGHT(RIGHT_ICON_ID, BAGL_GLYPH_ICON_RIGHT),

    // <<       >>
    // <Title>
    // <Partial>

    UI_TEXT(LINE_1_ID, 0, 12, 128, st_ctx.title),
    UI_TEXT(LINE_2_ID, 0, 26, 128, st_ctx.partial)};

// Step 8: Confirm
static const bagl_element_t ui_tx_confirm_step[] = {
    UI_BACKGROUND(), UI_ICON_LEFT(LEFT_ICON_ID, BAGL_GLYPH_ICON_LEFT),
    UI_ICON_RIGHT(RIGHT_ICON_ID, BAGL_GLYPH_ICON_RIGHT),

    // <<       >>
    //    Confirm
    //    <Check>

    UI_TEXT(LINE_1_ID, 0, 12, 128, "Confirm"),
    UI_ICON(LINE_2_ID, 0, 24, 128, BAGL_GLYPH_ICON_CHECK)};

// Step 9: Deny
static const bagl_element_t ui_tx_deny_step[] = {
    UI_BACKGROUND(), UI_ICON_LEFT(LEFT_ICON_ID, BAGL_GLYPH_ICON_LEFT),

    // <<       ()
    //    Deny
    //      X

    UI_TEXT(LINE_1_ID, 0, 12, 128, "Deny"),
    UI_ICON(LINE_2_ID, 0, 24, 128, BAGL_GLYPH_ICON_CROSS)};

// Step 1: Transaction Summary
unsigned int ui_tx_summary_step_button(unsigned int button_mask,
                                       unsigned int __attribute__((unused))
                                       button_mask_counter) {
    switch (button_mask) {
        case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
            if (st_ctx.type == Verify) { // Verify skips to Senders
                st_ctx.step = Senders;
                st_ctx.display_index = 1;
                update_display_count();
                reformat_senders();
                shift_display();
            } else { // Other flows all go to Operator
                st_ctx.step = Operator;
                st_ctx.display_index = 1;
                update_display_count();
                reformat_operator();
                shift_display();
            }
            UX_DISPLAY(ui_tx_intermediate_step, NULL);
            break;
    }

    return 0;
}

void handle_intermediate_left_press() {
    // Navigate Left (scroll or return to previous step)
    switch (st_ctx.step) {
        // All Flows with displayed Operator return to Summary
        case Operator: {
            if (first_screen()) {
                st_ctx.step = Summary;
                st_ctx.display_index = 1;
                UX_DISPLAY(ui_tx_summary_step, NULL);
            } else { // Scroll Left
                st_ctx.display_index--;
                update_display_count();
                reformat_operator();
                shift_display();
                UX_REDISPLAY();
            }
        } break;

        // Verify returns to Sumamry
        // All others return to Operator
        case Senders: {
            if (first_screen()) {
                if (st_ctx.type == Verify) {
                    st_ctx.step = Summary;
                    st_ctx.display_index = 1;
                    UX_DISPLAY(ui_tx_summary_step, NULL);
                } else {
                    st_ctx.step = Operator;
                    st_ctx.display_index = 1;
                    update_display_count();
                    reformat_operator();
                    shift_display();
                }
            } else { // Scroll Left
                st_ctx.display_index--;
                update_display_count();
                reformat_senders();
                shift_display();
            }
            UX_REDISPLAY();
        } break;

        // Create, Update, Transfer return to Senders
        // Other flows do not have Recipients
        case Recipients: {
            if (first_screen()) {
                if ((st_ctx.type == Create || st_ctx.type == Update) &&
                    st_ctx.transaction.data.cryptoCreateAccount
                            .which_staked_id !=
                        Hedera_CryptoCreateTransactionBody_staked_account_id_tag &&
                    st_ctx.transaction.data.cryptoCreateAccount
                            .which_staked_id !=
                        Hedera_CryptoCreateTransactionBody_staked_node_id_tag &&
                    st_ctx.transaction.data.cryptoUpdateAccount
                            .which_staked_id !=
                        Hedera_CryptoUpdateTransactionBody_staked_account_id_tag &&
                    st_ctx.transaction.data.cryptoUpdateAccount
                            .which_staked_id !=
                        Hedera_CryptoUpdateTransactionBody_staked_node_id_tag) {
                    st_ctx.step = Operator;
                    st_ctx.display_index = 1;
                    update_display_count();
                    reformat_operator();
                    shift_display();
                } else {
                    st_ctx.step = Senders;
                    st_ctx.display_index = 1;
                    update_display_count();
                    reformat_senders();
                    shift_display();
                }
            } else { // Scroll Left
                st_ctx.display_index--;
                update_display_count();
                reformat_recipients();
                shift_display();
            }
            UX_REDISPLAY();
        } break;

        // Create, Update, Transfer return to Recipients
        // Mint, Burn return to Senders
        // Other flows do not have Amount
        case Amount: {
            if (first_screen()) {
                if (st_ctx.type == Transfer || st_ctx.type == TokenTransfer ||
                    st_ctx.type == Create ||
                    st_ctx.type == Update) { // Return to Recipients
                    st_ctx.step = Recipients;
                    st_ctx.display_index = 1;
                    update_display_count();
                    reformat_recipients();
                    shift_display();
                } else if (st_ctx.type == TokenMint ||
                           st_ctx.type == TokenBurn) { // Return to Senders
                    st_ctx.step = Senders;
                    st_ctx.display_index = 1;
                    update_display_count();
                    reformat_senders();
                    shift_display();
                }
            } else { // Scroll left
                st_ctx.display_index--;
                update_display_count();
                reformat_amount();
                shift_display();
            }
            UX_REDISPLAY();
        } break;

        // Create, Update, Transfer, Mint, Burn return to Amount
        // Associate, Dissociate return to Senders
        case Fee: {
            if (first_screen()) { // Return to Senders
                if (st_ctx.type == Associate || st_ctx.type == Dissociate) {
                    st_ctx.step = Senders;
                    st_ctx.display_index = 1;
                    update_display_count();
                    reformat_senders();
                    shift_display();
                } else { // Return to Amount
                    st_ctx.step = Amount;
                    st_ctx.display_index = 1;
                    update_display_count();
                    reformat_amount();
                    shift_display();
                }
            } else { // Scroll left
                st_ctx.display_index--;
                update_display_count();
                reformat_fee();
                shift_display();
            }
            UX_REDISPLAY();
        } break;

        // All flows return to Fee from Memo
        case Memo: {
            if (first_screen()) { // Return to Fee
                st_ctx.step = Fee;
                st_ctx.display_index = 1;
                update_display_count();
                reformat_fee();
                shift_display();
            } else { // Scroll Left
                st_ctx.display_index--;
                update_display_count();
                reformat_memo();
                shift_display();
            }
            UX_REDISPLAY();
        } break;

        case Summary:
        case Confirm:
        case Deny:
            // this handler does not apply to these steps
            break;
    }
}

void handle_intermediate_right_press() {
    // Navigate Right (scroll or continue to next step)
    switch (st_ctx.step) {
        // All flows proceed from Operator to Senders
        case Operator: {
            if (last_screen()) { // Continue to Senders
                if ((st_ctx.type == Create || st_ctx.type == Update) &&
                    st_ctx.transaction.data.cryptoCreateAccount
                            .which_staked_id !=
                        Hedera_CryptoCreateTransactionBody_staked_account_id_tag &&
                    st_ctx.transaction.data.cryptoCreateAccount
                            .which_staked_id !=
                        Hedera_CryptoCreateTransactionBody_staked_node_id_tag &&
                    st_ctx.transaction.data.cryptoUpdateAccount
                            .which_staked_id !=
                        Hedera_CryptoUpdateTransactionBody_staked_account_id_tag &&
                    st_ctx.transaction.data.cryptoUpdateAccount
                            .which_staked_id !=
                        Hedera_CryptoUpdateTransactionBody_staked_node_id_tag) {
                    st_ctx.step = Amount;
                    st_ctx.display_index = 1;
                    update_display_count();
                    reformat_amount();
                    shift_display();
                } else {
                    st_ctx.step = Senders;
                    st_ctx.display_index = 1;
                    update_display_count();
                    reformat_senders();
                    shift_display();
                }
            } else { // Scroll Right
                st_ctx.display_index++;
                update_display_count();
                reformat_operator();
                shift_display();
            }
            UX_REDISPLAY();
        } break;

        // Verify continues to Confirm
        // Mint, Burn continue to Amount
        // Create, Update, Transfer continue to Recipients
        // Associate, Dissociate continues to Fee
        case Senders: {
            if (last_screen()) {
                if (st_ctx.type == Verify) { // Continue to Confirm
                    st_ctx.step = Confirm;
                    UX_DISPLAY(ui_tx_confirm_step, NULL);
                } else if (st_ctx.type == TokenMint ||
                           st_ctx.type == TokenBurn) { // Continue to Amount
                    st_ctx.step = Amount;
                    st_ctx.display_index = 1;
                    update_display_count();
                    reformat_amount();
                    shift_display();
                } else if (st_ctx.type == Create || st_ctx.type == Transfer ||
                           st_ctx.type == TokenTransfer ||
                           st_ctx.type == Update) { // Continue to Recipients
                    st_ctx.step = Recipients;
                    st_ctx.display_index = 1;
                    update_display_count();
                    reformat_recipients();
                    shift_display();
                } else if (st_ctx.type == Associate ||
                           st_ctx.type == Dissociate) { // Continue to Fee
                    st_ctx.step = Fee;
                    st_ctx.display_index = 1;
                    update_display_count();
                    reformat_fee();
                    shift_display();
                }
            } else { // Scroll Right
                st_ctx.display_index++;
                update_display_count();
                reformat_senders();
                shift_display();
            }
            UX_REDISPLAY();
        } break;

        // All flows with Recipients continue to Amount
        case Recipients: {
            if (last_screen()) { // Continue to Amount
                st_ctx.step = Amount;
                st_ctx.display_index = 1;
                update_display_count();
                reformat_amount();
                shift_display();
            } else { // Scroll Right
                st_ctx.display_index++;
                update_display_count();
                reformat_recipients();
                shift_display();
            }
            UX_REDISPLAY();
        } break;

        // All flows with Amounts continue to Fee
        case Amount: {
            if (last_screen()) { // Continue to Fee
                st_ctx.step = Fee;
                st_ctx.display_index = 1;
                update_display_count();
                reformat_fee();
                shift_display();
            } else { // Scroll Right
                st_ctx.display_index++;
                update_display_count();
                reformat_amount();
                shift_display();
            }
            UX_REDISPLAY();
        } break;

        // Always to Memo
        case Fee: {
            if (last_screen()) { // Continue to Memo
                st_ctx.step = Memo;
                st_ctx.display_index = 1;
                update_display_count();
                reformat_memo();
                shift_display();
            } else { // Scroll Right
                st_ctx.display_index++;
                update_display_count();
                reformat_fee();
                shift_display();
            }
            UX_REDISPLAY();
        } break;

        // Always to Confirm
        case Memo: {
            if (last_screen()) { // Continue to Confirm
                st_ctx.step = Confirm;
                st_ctx.display_index = 1;
                UX_DISPLAY(ui_tx_confirm_step, NULL);
            } else { // Scroll Right
                st_ctx.display_index++;
                update_display_count();
                reformat_memo();
                shift_display();
                UX_REDISPLAY();
            }
        } break;

        case Summary:
        case Confirm:
        case Deny:
            // this handler does not apply to these steps
            break;
    }
}

// Step 2 - 7: Operator, Senders, Recipients, Amount, Fee, Memo
unsigned int ui_tx_intermediate_step_button(unsigned int button_mask,
                                            unsigned int __attribute__((unused))
                                            button_mask_counter) {
    switch (button_mask) {
        case BUTTON_EVT_RELEASED | BUTTON_LEFT:
            handle_intermediate_left_press();
            break;
        case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
            handle_intermediate_right_press();
            break;
        case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
            // Skip to confirm screen
            st_ctx.step = Confirm;
            UX_DISPLAY(ui_tx_confirm_step, NULL);
            break;
    }

    return 0;
}

unsigned int ui_tx_confirm_step_button(unsigned int button_mask,
                                       unsigned int __attribute__((unused))
                                       button_mask_counter) {
    switch (button_mask) {
        case BUTTON_EVT_RELEASED | BUTTON_LEFT:
            if (st_ctx.type == Verify) { // Return to Senders
                st_ctx.step = Senders;
                st_ctx.display_index = 1;
                update_display_count();
                reformat_senders();
                shift_display();
            } else { // Return to Memo
                st_ctx.step = Memo;
                st_ctx.display_index = 1;
                update_display_count();
                reformat_memo();
                shift_display();
            }
            UX_DISPLAY(ui_tx_intermediate_step, NULL);
            break;
        case BUTTON_EVT_RELEASED | BUTTON_RIGHT:
            // Continue to Deny
            st_ctx.step = Deny;
            UX_DISPLAY(ui_tx_deny_step, NULL);
            break;
        case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
            // Exchange Signature (OK)
            io_exchange_with_code(EXCEPTION_OK, 64);
            ui_idle();
            break;
    }

    return 0;
}

unsigned int ui_tx_deny_step_button(unsigned int button_mask,
                                    unsigned int __attribute__((unused))
                                    button_mask_counter) {
    switch (button_mask) {
        case BUTTON_EVT_RELEASED | BUTTON_LEFT:
            // Return to Confirm
            st_ctx.step = Confirm;
            UX_DISPLAY(ui_tx_confirm_step, NULL);
            break;
        case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT:
            // Reject
            st_ctx.step = Unknown;
            io_exchange_with_code(EXCEPTION_USER_REJECTED, 0);
            ui_idle();
            break;
    }

    return 0;
}

#elif defined(TARGET_NANOX) || defined(TARGET_NANOS2)

// UI Definition for Nano X

// Confirm Callback
unsigned int io_seproxyhal_tx_approve(const bagl_element_t* e) {
    UNUSED(e);
    io_exchange_with_code(EXCEPTION_OK, 64);
    ui_idle();
    return 0;
}

// Reject Callback
unsigned int io_seproxyhal_tx_reject(const bagl_element_t* e) {
    UNUSED(e);
    io_exchange_with_code(EXCEPTION_USER_REJECTED, 0);
    ui_idle();
    return 0;
}

UX_STEP_NOCB(summary_step, bnn,
             {"Summary", st_ctx.summary_line_1, st_ctx.summary_line_2});

UX_STEP_NOCB(
    operator_step,
    bnnn_paging,
    {
        .title = "Operator",
        .text = (char*) st_ctx.operator
    }
);

UX_STEP_NOCB(senders_step, bnnn_paging,
             {.title = (char*)st_ctx.senders_title,
              .text = (char*)st_ctx.senders});

UX_STEP_NOCB(recipients_step, bnnn_paging,
             {.title = (char*)st_ctx.recipients_title,
              .text = (char*)st_ctx.recipients});

UX_STEP_NOCB(amount_step, bnnn_paging,
             {.title = (char*)st_ctx.amount_title,
              .text = (char*)st_ctx.amount});

UX_STEP_NOCB(fee_step, bnnn_paging,
             {.title = "Max Fee", .text = (char*)st_ctx.fee});

UX_STEP_NOCB(memo_step, bnnn_paging,
             {.title = "Memo", .text = (char*)st_ctx.memo});

UX_STEP_VALID(confirm_step, pb, io_seproxyhal_tx_approve(NULL),
              {&C_icon_validate_14, "Confirm"});

UX_STEP_VALID(reject_step, pb, io_seproxyhal_tx_reject(NULL),
              {&C_icon_crossmark, "Reject"});

// Transfer UX Flow
UX_DEF(ux_transfer_flow, &summary_step, &operator_step, &senders_step,
       &recipients_step, &amount_step, &fee_step, &memo_step, &confirm_step,
       &reject_step);

// Verify UX Flow
UX_DEF(ux_verify_flow, &summary_step, &senders_step, &confirm_step,
       &reject_step);

// Burn/Mint UX Flow
UX_DEF(ux_burn_mint_flow, &summary_step, &operator_step, &senders_step,
       &amount_step, &fee_step, &memo_step, &confirm_step, &reject_step);

// Associate UX Flow
UX_DEF(ux_associate_flow, &summary_step, &operator_step, &senders_step,
       &fee_step, &memo_step, &confirm_step, &reject_step);

#elif defined(HAVE_NBGL)

static void review_choice(bool confirm) {
    // Answer, display a status page and go back to main
    if (confirm) {
        io_exchange_with_code(EXCEPTION_OK, 64);
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_SIGNED, ui_idle);
    } else {
        io_exchange_with_code(EXCEPTION_USER_REJECTED, 0);
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_REJECTED, ui_idle);
    }
}

// Max is 7 infos for transfer transaction
// If a new flow is added or flows are modified to include more steps, don't
// forget to update the infos array size!
static nbgl_contentTagValue_t infos[7];
// Content of the review flow
static nbgl_contentTagValueList_t content;
static char review_start_title[64];
static char review_final_title[64];

static void create_transaction_flow(void) {
    uint8_t index = 0;
    infos[index].value = st_ctx.summary_line_1;
    snprintf(review_start_title, sizeof(review_start_title),
             "Review transaction to\n%s", st_ctx.summary_line_1);
    snprintf(review_final_title, sizeof(review_final_title),
             "Sign transaction to\n%s", st_ctx.summary_line_1);

    infos[index].item = "With key";
    infos[index].value = st_ctx.summary_line_2;
    ++index;

    switch (st_ctx.type) {
        case Verify:
            // FALLTHROUGH
        case Associate:
            infos[index].item = st_ctx.senders_title;
            infos[index].value = st_ctx.senders;
            ++index;
            break;
        case Create:
            infos[index].item = "Operator";
            infos[index].value = st_ctx.operator;
            ++index;
            infos[index].item = st_ctx.amount_title;
            infos[index].value = st_ctx.amount;
            ++index;
            infos[index].item = "Max Fee";
            infos[index].value = st_ctx.fee;
            ++index;
            infos[index].item = "Memo";
            infos[index].value = st_ctx.memo;
            ++index;
            break;
        case TokenTransfer:
            // FALLTHROUGH
        case Transfer:
            infos[index].item = "Operator";
            infos[index].value = st_ctx.operator;
            ++index;
            infos[index].item = st_ctx.senders_title;
            infos[index].value = st_ctx.senders;
            ++index;
            infos[index].item = "Recipient";
            infos[index].value = st_ctx.recipients;
            ++index;
            infos[index].item = st_ctx.amount_title;
            infos[index].value = st_ctx.amount;
            ++index;
            infos[index].item = "Max Fee";
            infos[index].value = st_ctx.fee;
            ++index;
            infos[index].item = "Memo";
            infos[index].value = st_ctx.memo;
            ++index;
            break;
        case TokenMint:
            // FALLTHROUGH
        case TokenBurn:
            infos[index].item = st_ctx.senders_title;
            infos[index].value = st_ctx.senders;
            ++index;
            infos[index].item = st_ctx.amount_title;
            infos[index].value = st_ctx.amount;
            ++index;
            break;
        default:
            // Unreachable
            ;
    }

    // If a new flow is added or flows are modified to include more steps, don't
    // forget to update the infos array size!
    content.nbMaxLinesForValue = 0;
    content.smallCaseForValue = true;
    content.wrapping = true;
    content.pairs = infos;
    content.callback = NULL;
    content.startIndex = 0;
    content.nbPairs = index;
}
#endif

// Common for all devices

void ui_sign_transaction(void) {
#if defined(TARGET_NANOS)

    UX_DISPLAY(ui_tx_summary_step, NULL);

#elif defined(TARGET_NANOX) || defined(TARGET_NANOS2)

    switch (st_ctx.type) {
        case Associate:
            // FALLTHROUGH
        case Dissociate:
            ux_flow_init(0, ux_associate_flow, NULL);
            break;
        case Verify:
            ux_flow_init(0, ux_verify_flow, NULL);
            break;
        case Create:
            // FALLTHROUGH
        case Update:
            // FALLTHROUGH
        case TokenTransfer:
            // FALLTHROUGH
        case Transfer:
            ux_flow_init(0, ux_transfer_flow, NULL);
            break;
        case TokenMint:
            // FALLTHROUGH
        case TokenBurn:
            ux_flow_init(0, ux_burn_mint_flow, NULL);
            break;

        default:
            break;
    }

#elif defined(HAVE_NBGL)

    create_transaction_flow();

    // Start review
    nbgl_useCaseReview(TYPE_TRANSACTION, &content, &C_icon_hedera_64x64,
                       review_start_title, NULL, review_final_title,
                       review_choice);
#endif
}
