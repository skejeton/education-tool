#ifndef HPP_CATEDU_GUI_MAIN_MENU_MAIN_MENU
#define HPP_CATEDU_GUI_MAIN_MENU_MAIN_MENU

#include "catedu/core/storage/id_retainer.hpp"
#include "catedu/diag/diag_monitor.hpp"
#include "catedu/input.hpp"
#include "catedu/staging/ui/ui_user.hpp"
#include "catedu/ui/rendering/core.hpp"
#include "catedu/ui/rendering/font.hpp"
#include <sokol/sokol_app.h>

struct GuiMainMenu
{
    UiState ui_state;

    static GuiMainMenu init();

    void show();
};

#endif // HPP_CATEDU_GUI_MAIN_MENU_MAIN_MENU
