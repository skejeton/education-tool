#ifndef HPP_CATEDU_GUI_MAIN_MENU_MAIN_MENU
#define HPP_CATEDU_GUI_MAIN_MENU_MAIN_MENU

#include "catedu/ui/rendering/core.hpp"
#include "catedu/ui/rendering/font.hpp"

struct GuiMainMenu
{
    UiRenderingCore core;
    UiFontRenderer font;

    static GuiMainMenu init();
    void show();
};

#endif // HPP_CATEDU_GUI_MAIN_MENU_MAIN_MENU
