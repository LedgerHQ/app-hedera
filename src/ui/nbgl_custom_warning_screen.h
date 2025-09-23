#pragma once
#ifdef HAVE_NBGL

#include "nbgl_page.h"
#include "nbgl_use_case.h"
#include "ui_glyphs_helper.h"
#include "nbgl_use_case.h"

// Hedera: show a custom warning page with a QR icon, then continue to standard review
// - content: review tag/value list to display after the warning
// - icon: icon used in review pages
// - reviewTitle: title for the first review page
// - finishTitle: title for the last review page (long press)
// - warningTitle: title of the warning page
// - warningDescription: description text on the warning page
// - qrUrl: URL encoded in the QR modal (opened by top-right QR button)
// - qrCaption: optional caption under QR (pass NULL for none)
// - finalChoiceCb: callback invoked when the user accepts on the final review page
// - onReject: optional callback when user cancels at the warning page
void hedera_warning_then_review(const nbgl_contentTagValueList_t *content,
                                const nbgl_icon_details_t        *icon,
                                const char                       *reviewTitle,
                                const char                       *finishTitle,
                                const char                       *warningTitle,
                                const char                       *warningDescription,
                                const char                       *qrTitle,
                                const char                       *qrUrl,
                                const char                       *qrCaption,
                                nbgl_choiceCallback_t             finalChoiceCb);

#endif


