#pragma once

#include <printf.h>

#include "io.h"
#include "os.h"
#include "ux.h"

void ui_idle(void);

void ui_sign_transaction(void);

void ui_get_public_key(void);

#if defined(TARGET_NANOS)

// Common UI element definitions for Nano S
#define UI_BACKGROUND()                                    \
    {                                                      \
        {                                                  \
            BAGL_RECTANGLE, 0, 0,        0, 128, 32, 0, 0, \
            BAGL_FILL,      0, 0xFFFFFF, 0, 0},            \
            NULL                                           \
    }
#define UI_ICON_LEFT(userid, glyph) \
    { {BAGL_ICON, userid, 3, 12, 7, 7, 0, 0, 0, 0xFFFFFF, 0, 0, glyph}, NULL }
#define UI_ICON_RIGHT(userid, glyph) \
    { {BAGL_ICON, userid, 117, 13, 8, 6, 0, 0, 0, 0xFFFFFF, 0, 0, glyph}, NULL }
#define UI_TEXT(userid, x, y, w, text)                                  \
    {                                                                   \
        {BAGL_LABELINE,                                                 \
         userid,                                                        \
         x,                                                             \
         y,                                                             \
         w,                                                             \
         12,                                                            \
         0,                                                             \
         0,                                                             \
         0,                                                             \
         0xFFFFFF,                                                      \
         0,                                                             \
         BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, \
         0},                                                            \
            (char *)(text)                                              \
    }
#define UI_ICON(userid, x, y, w, glyph)                                 \
    {                                                                   \
        {BAGL_ICON,                                                     \
         userid,                                                        \
         x,                                                             \
         y,                                                             \
         w,                                                             \
         6,                                                             \
         0,                                                             \
         0,                                                             \
         0,                                                             \
         0xFFFFFF,                                                      \
         0,                                                             \
         BAGL_FONT_OPEN_SANS_REGULAR_11px | BAGL_FONT_ALIGNMENT_CENTER, \
         glyph},                                                        \
            NULL                                                        \
    }

#endif // TARGET
