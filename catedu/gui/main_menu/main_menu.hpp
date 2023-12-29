#ifndef HPP_CATEDU_GUI_MAIN_MENU_MAIN_MENU
#define HPP_CATEDU_GUI_MAIN_MENU_MAIN_MENU

#include "catedu/ui/user.hpp"
#include <sokol/sokol_app.h>

struct GuiMainMenu
{
    UiState *ui_state;

    static GuiMainMenu init(UiState *ui_state);
    void deinit();

    int show();
};

#endif // HPP_CATEDU_GUI_MAIN_MENU_MAIN_MENU
