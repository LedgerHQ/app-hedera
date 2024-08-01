#include "glyphs.h"
#include "ui_common.h"
#include "utils.h"
#include "ux.h"

#ifdef HAVE_NBGL
#include "nbgl_page.h"
#include "nbgl_use_case.h"
#endif

/*
 * Defines the main menu and idle actions for the app
 */

#if defined(TARGET_NANOS)

ux_state_t ux;
unsigned int ux_step;
unsigned int ux_step_count;

static const ux_menu_entry_t menu_main[4];

static const ux_menu_entry_t menu_about[3] = {{
                                                  .menu = NULL,
                                                  .callback = NULL,
                                                  .userid = 0,
                                                  .icon = NULL,
                                                  .line1 = "Version",
                                                  .line2 = APPVERSION,
                                                  .text_x = 0,
                                                  .icon_x = 0,
                                              },

                                              {
                                                  .menu = menu_main,
                                                  .callback = NULL,
                                                  .userid = 0,
                                                  .icon = &C_icon_back,
                                                  .line1 = "Back",
                                                  .line2 = NULL,
                                                  .text_x = 61,
                                                  .icon_x = 40,
                                              },

                                              UX_MENU_END};

static const ux_menu_entry_t menu_main[4] = {
    {.menu = NULL,
     .callback = NULL,
     .userid = 0,
     .icon = NULL,
     .line1 = "Awaiting",
     .line2 = "Commands",
     .text_x = 0,
     .icon_x = 0},
    {
        .menu = menu_about,
        .callback = NULL,
        .userid = 0,
        .icon = NULL,
        .line1 = "About",
        .line2 = NULL,
        .text_x = 0,
        .icon_x = 0,
    },

    {
        .menu = NULL,
        .callback = (void (*)(unsigned int)) & os_sched_exit,
        .userid = 0,
        .icon = &C_icon_dashboard,
        .line1 = "Quit app",
        .line2 = NULL,
        .text_x = 50,
        .icon_x = 29,
    },

    UX_MENU_END};

#elif defined(TARGET_NANOX) || defined(TARGET_NANOS2)

UX_STEP_NOCB(ux_idle_flow_1_step, nn, {"Awaiting", "Commands"});

UX_STEP_NOCB(ux_idle_flow_2_step, bn,
             {
                 "Version",
                 APPVERSION,
             });

UX_STEP_VALID(ux_idle_flow_3_step, pb, os_sched_exit(-1),
              {&C_icon_dashboard_x, "Exit"});

UX_DEF(ux_idle_flow, &ux_idle_flow_1_step, &ux_idle_flow_2_step,
       &ux_idle_flow_3_step);

#elif defined(TARGET_STAX) || defined(TARGET_FLEX)

#define SETTING_INFO_NB 2
static const char* const info_types[SETTING_INFO_NB] = {"Version", "Developer"};
static const char* const info_contents[SETTING_INFO_NB] = {APPVERSION,
                                                           "(c) 2024 Ledger"};

static const nbgl_contentInfoList_t infoList = {
    .nbInfos = SETTING_INFO_NB,
    .infoTypes = info_types,
    .infoContents = info_contents,
};

static void quit_app_callback(void) { os_sched_exit(-1); }

static void ui_idle_nbgl(void) {
    nbgl_useCaseHomeAndSettings(APPNAME, &C_icon_hedera_64x64, NULL,
                                INIT_HOME_PAGE, NULL, &infoList, NULL,
                                quit_app_callback);
}
#endif

// Common for all devices

void ui_idle(void) {
#if defined(TARGET_NANOS)

    UX_MENU_DISPLAY(0, menu_main, NULL);

#elif defined(TARGET_NANOX) || defined(TARGET_NANOS2)

    if (G_ux.stack_count == 0) {
        ux_stack_push();
    }
    ux_flow_init(0, ux_idle_flow, NULL);

#elif defined(TARGET_STAX) || defined(TARGET_FLEX)

    ui_idle_nbgl();

#endif
}
