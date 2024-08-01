#include "get_public_key.h"
#include "glyphs.h"
#include "ui_common.h"
#include "ux.h"

#ifdef HAVE_NBGL
#include "nbgl_use_case.h"
#endif

#if defined(TARGET_NANOS)

static const bagl_element_t ui_get_public_key_compare[] = {
    UI_BACKGROUND(), UI_ICON_LEFT(LEFT_ICON_ID, BAGL_GLYPH_ICON_LEFT),
    UI_ICON_RIGHT(RIGHT_ICON_ID, BAGL_GLYPH_ICON_RIGHT),
    // <=                  =>
    //      Public Key
    //      <partial>
    //
    UI_TEXT(LINE_1_ID, 0, 12, 128, "Public Key"),
    UI_TEXT(LINE_2_ID, 0, 26, 128, gpk_ctx.partial_key)};

static const bagl_element_t ui_get_public_key_approve[] = {
    UI_BACKGROUND(),
    UI_ICON_LEFT(LEFT_ICON_ID, BAGL_GLYPH_ICON_CROSS),
    UI_ICON_RIGHT(RIGHT_ICON_ID, BAGL_GLYPH_ICON_CHECK),
    //
    //    Export Public
    //       Key #123?
    //
    UI_TEXT(LINE_1_ID, 0, 12, 128, "Export Public"),
    UI_TEXT(LINE_2_ID, 0, 26, 128, gpk_ctx.ui_approve_l2),
};

static void shift_partial_key() {
    memmove(gpk_ctx.partial_key, gpk_ctx.full_key + gpk_ctx.display_index,
            DISPLAY_SIZE);
}

static unsigned int ui_get_public_key_compare_button(
    unsigned int button_mask, unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
        case BUTTON_LEFT: // Left
        case BUTTON_EVT_FAST | BUTTON_LEFT:
            if (gpk_ctx.display_index > 0) gpk_ctx.display_index--;
            shift_partial_key();
            UX_REDISPLAY();
            break;
        case BUTTON_RIGHT: // Right
        case BUTTON_EVT_FAST | BUTTON_RIGHT:
            if (gpk_ctx.display_index < KEY_SIZE - DISPLAY_SIZE)
                gpk_ctx.display_index++;
            shift_partial_key();
            UX_REDISPLAY();
            break;
        case BUTTON_EVT_RELEASED | BUTTON_LEFT | BUTTON_RIGHT: // Continue
            ui_idle();
            break;
    }
    return 0;
}

static const bagl_element_t* ui_prepro_get_public_key_compare(
    const bagl_element_t* element) {
    if (element->component.userid == LEFT_ICON_ID && gpk_ctx.display_index == 0)
        return NULL; // Hide Left Arrow at Left Edge
    if (element->component.userid == RIGHT_ICON_ID &&
        gpk_ctx.display_index == KEY_SIZE - DISPLAY_SIZE)
        return NULL; // Hide Right Arrow at Right Edge
    return element;
}

static void compare_pk() {
    // init partial key str from full str
    memmove(gpk_ctx.partial_key, gpk_ctx.full_key, DISPLAY_SIZE);
    gpk_ctx.partial_key[DISPLAY_SIZE] = '\0';

    // init display index
    gpk_ctx.display_index = 0;

    // Display compare with button mask
    UX_DISPLAY(ui_get_public_key_compare, ui_prepro_get_public_key_compare);
}

static unsigned int ui_get_public_key_approve_button(
    unsigned int button_mask, unsigned int button_mask_counter) {
    UNUSED(button_mask_counter);
    switch (button_mask) {
        case BUTTON_EVT_RELEASED | BUTTON_LEFT: // REJECT
            io_exchange_with_code(EXCEPTION_USER_REJECTED, 0);
            ui_idle();
            break;

        case BUTTON_EVT_RELEASED | BUTTON_RIGHT: // APPROVE
            io_exchange_with_code(EXCEPTION_OK, 32);
            compare_pk();
            break;

        default:
            break;
    }

    return 0;
}

#elif defined(TARGET_NANOX) || defined(TARGET_NANOS2)

UX_STEP_CB(ux_compare_pk_flow_1_step, bnnn_paging, ui_idle(),
           {.title = "Public Key", .text = (char *)gpk_ctx.full_key});

UX_DEF(ux_compare_pk_flow, &ux_compare_pk_flow_1_step);

static void compare_pk() { ux_flow_init(0, ux_compare_pk_flow, NULL); }

static unsigned int pk_approved() {
    io_exchange_with_code(EXCEPTION_OK, 32);
    compare_pk();
    return 0;
}

static unsigned int pk_rejected() {
    io_exchange_with_code(EXCEPTION_USER_REJECTED, 0);
    ui_idle();
    return 0;
}

UX_STEP_NOCB(ux_approve_pk_flow_1_step, bn,
             {"Export Public", gpk_ctx.ui_approve_l2});

UX_STEP_VALID(ux_approve_pk_flow_2_step, pb, pk_approved(),
              {&C_icon_validate_14, "Approve"});

UX_STEP_VALID(ux_approve_pk_flow_3_step, pb, pk_rejected(),
              {&C_icon_crossmark, "Reject"});

UX_DEF(ux_approve_pk_flow, &ux_approve_pk_flow_1_step,
       &ux_approve_pk_flow_2_step, &ux_approve_pk_flow_3_step);

#elif defined(HAVE_NBGL)

static void callback_match(bool match) {
    if (match) {
        io_exchange_with_code(EXCEPTION_OK, 32);
    } else {
        io_exchange_with_code(EXCEPTION_USER_REJECTED, 0);
    }
    ui_idle();
}

static void callback_export(bool accept) {
    if (accept) {
        nbgl_useCaseAddressConfirmation((const char *)gpk_ctx.full_key,
                                        callback_match);
    } else {
        io_exchange_with_code(EXCEPTION_USER_REJECTED, 0);
        ui_idle();
    }
}

static void ui_get_public_key_nbgl(void) {
    nbgl_useCaseChoice(&C_icon_hedera_64x64, "Export Public Key?",
                       gpk_ctx.ui_approve_l2, "Allow", "Don't allow",
                       callback_export);
}

#endif // TARGET

// Common for all devices

void ui_get_public_key(void) {
#if defined(TARGET_NANOS)

    UX_DISPLAY(ui_get_public_key_approve, NULL);

#elif defined(TARGET_NANOX) || defined(TARGET_NANOS2)

    ux_flow_init(0, ux_approve_pk_flow, NULL);

#elif defined(HAVE_NBGL)

    ui_get_public_key_nbgl();

#endif // #if TARGET_
}
