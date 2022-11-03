#include "ui_common.h"

#ifdef HAVE_NBGL
#include "nbgl_page.h"
#endif

#ifdef HAVE_NBGL
void releaseContext(void) {
    if (pageContext != NULL) {
        nbgl_pageRelease(pageContext);
        pageContext = NULL;
    }
}
#endif
