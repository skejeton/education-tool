#ifndef HPP_CATEDU_GUI_MAIN_MENU_MAIN_MENU
#define HPP_CATEDU_GUI_MAIN_MENU_MAIN_MENU

#include "catedu/core/storage/id_retainer.hpp"
#include "catedu/ui/rendering/core.hpp"
#include "catedu/ui/rendering/font.hpp"

struct GuiTool
{
    int v;
};

struct GuiMainMenu
{
    UiRenderingCore* core;
    UiFontRenderer font;
    IdRetainer<GuiTool> tools;

    static GuiMainMenu init();
    void show(Vector2 mouse_pos);
};

#endif // HPP_CATEDU_GUI_MAIN_MENU_MAIN_MENU
