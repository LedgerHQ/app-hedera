#pragma once

#include "glyphs.h"

// Unify warning icon across devices:
// - NBGL (Stax/Flex): use 64px warning icon provided by NBGL assets
// - BAGL (Nano S+/X): use 14px app-provided warning icon
#ifdef HAVE_NBGL
    #define ICON_APP_WARNING C_Warning_64px
#else
    #define ICON_APP_WARNING C_icon_warning
#endif
