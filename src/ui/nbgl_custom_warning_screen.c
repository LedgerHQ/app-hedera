// Hedera custom warning screen with QR, followed by standard review
#ifdef HAVE_NBGL
#include "nbgl_custom_warning_screen.h"
#include "nbgl_page.h"
#include "glyphs.h"
#include "ui_common.h"

typedef struct {
    const nbgl_contentTagValueList_t *content;
    const nbgl_icon_details_t        *icon;
    const char                       *reviewTitle;
    const char                       *finishTitle;
    nbgl_choiceCallback_t             finalChoiceCb;
    const char                       *warningTitle;
    const char                       *warningDescription;
    const char                       *qrTitle;
    const char                       *qrUrl;
    const char                       *qrCaption;
} hedera_warning_ctx_t;

enum {
    HEDERA_WARN_CHOICE_TOKEN = FIRST_USER_TOKEN,
    HEDERA_WARN_QR_TOKEN,
};

static hedera_warning_ctx_t g_ctx;
static nbgl_layout_t       *g_qrModal;

static void hedera_qr_modal_cb(int token, uint8_t index)
{
    UNUSED(token);
    UNUSED(index);
    if (g_qrModal != NULL) {
        nbgl_layoutRelease(g_qrModal);
        g_qrModal = NULL;
        nbgl_screenRedraw();
        nbgl_refresh();
    }
}

static void hedera_show_qr_modal(void)
{
    char qrUrl[128] = {0};
    snprintf(qrUrl, sizeof(qrUrl), "https://%s", g_ctx.qrUrl);
    
    nbgl_layoutDescription_t layoutDescription
        = {.modal = true, .withLeftBorder = true, .onActionCallback = &hedera_qr_modal_cb, .tapActionText = NULL};
    nbgl_layoutHeader_t headerDesc
        = {.type = HEADER_BACK_AND_TEXT, .separationLine = true, .backAndText.token = 0, .backAndText.tuneId = TUNE_TAP_CASUAL, .backAndText.text = g_ctx.qrTitle};
    nbgl_layoutQRCode_t qrCode
        = {.url = qrUrl, .text1 = g_ctx.qrUrl, .text2 = g_ctx.qrCaption, .centered = true, .offsetY = 0};

    g_qrModal = nbgl_layoutGet(&layoutDescription);
    nbgl_layoutAddHeader(g_qrModal, &headerDesc);
    nbgl_layoutAddQRCode(g_qrModal, &qrCode);
    nbgl_layoutDraw(g_qrModal);
    nbgl_refresh();
}

static void hedera_warn_action_cb(int token, uint8_t index)
{
    if (token == HEDERA_WARN_QR_TOKEN) {
        hedera_show_qr_modal();
        return;
    }
    if (token == HEDERA_WARN_CHOICE_TOKEN) {
        if (index == 0) {
            // Continue -> start review
            nbgl_useCaseReview(TYPE_TRANSACTION,
                               g_ctx.content,
                               g_ctx.icon,
                               g_ctx.reviewTitle,
                               NULL,
                               g_ctx.finishTitle,
                               g_ctx.finalChoiceCb);
        } else {
            // Cancel
            if (g_ctx.finalChoiceCb != NULL) {
                g_ctx.finalChoiceCb(false);
            } else {
                nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_REJECTED, ui_idle);
            }
        }
    }
}

void hedera_warning_then_review(const nbgl_contentTagValueList_t *content,
                                const nbgl_icon_details_t        *icon,
                                const char                       *reviewTitle,
                                const char                       *finishTitle,
                                const char                       *warningTitle,
                                const char                       *warningDescription,
                                const char                       *qrTitle,
                                const char                       *qrUrl,
                                const char                       *qrCaption,
                                nbgl_choiceCallback_t             finalChoiceCb)
{
    g_ctx.content            = content;
    g_ctx.icon               = icon;
    g_ctx.reviewTitle        = reviewTitle;
    g_ctx.finishTitle        = finishTitle;
    g_ctx.finalChoiceCb      = finalChoiceCb;
    g_ctx.warningTitle       = warningTitle;
    g_ctx.warningDescription = warningDescription;
    g_ctx.qrTitle            = qrTitle;
    g_ctx.qrUrl              = qrUrl;
    g_ctx.qrCaption          = qrCaption;

    nbgl_layout_t *layoutCtx = NULL;
    nbgl_layoutDescription_t layoutDescription = {0};
    nbgl_layoutHeader_t headerDesc             = {.type = HEADER_EMPTY, .separationLine = false, .emptySpace.height = MEDIUM_CENTERING_HEADER};
    nbgl_contentCenter_t info                  = {0};
    nbgl_layoutChoiceButtons_t buttonsInfo
        = {.topText = "Continue", .bottomText = "Cancel", .token = HEDERA_WARN_CHOICE_TOKEN, .style = ROUNDED_AND_FOOTER_STYLE, .tuneId = TUNE_TAP_CASUAL};

    layoutDescription.withLeftBorder   = true;
    layoutDescription.onActionCallback = hedera_warn_action_cb;
    layoutCtx                          = nbgl_layoutGet(&layoutDescription);

    nbgl_layoutAddHeader(layoutCtx, &headerDesc);
    nbgl_layoutAddTopRightButton(layoutCtx, &QRCODE_ICON, HEDERA_WARN_QR_TOKEN, TUNE_TAP_CASUAL);

    info.title       = g_ctx.warningTitle;
    info.description = g_ctx.warningDescription;
    info.icon        = &C_Important_Circle_64px;
    nbgl_layoutAddContentCenter(layoutCtx, &info);

    nbgl_layoutAddChoiceButtons(layoutCtx, &buttonsInfo);
    nbgl_layoutDraw(layoutCtx);
    nbgl_refresh();
}

#endif