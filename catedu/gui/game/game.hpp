#pragma once
#include "catedu/ui/user.hpp"
#include <sokol/sokol_app.h>

struct DialogButton
{
    char *text;
    char *script;
};

struct Dialog
{
    char *title;
    char *text;
    char *translation;
    DialogButton buttons[8];
    bool open;
};

struct GuiGame
{
    UiState *ui_state;
    Dialog dialog;

    static GuiGame init(UiState *ui_state);
    void deinit();

    void show_dialog(Dialog dialog);

    int show(int currency, int reals, char **script);
};
