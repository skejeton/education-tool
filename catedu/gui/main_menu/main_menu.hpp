#pragma once
#include "catedu/ui/user.hpp"
#include <sokol/sokol_app.h>

struct GuiMainMenu
{
    enum
    {
        Normal,
        Settings,
        Exit,
    } state;
    UiState *ui_state;

    static GuiMainMenu init(UiState *ui_state);
    void deinit();

    int show();
};
