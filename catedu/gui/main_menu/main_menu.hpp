#pragma once
#include "catedu/ui/user.hpp"
#include <sokol/sokol_app.h>

struct GuiMainMenu
{
    UiState *ui_state;

    static GuiMainMenu init(UiState *ui_state);
    void deinit();

    int show();
};
