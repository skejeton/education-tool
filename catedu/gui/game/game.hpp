#ifndef HPP_CATEDU_GUI_GAME_GAME
#define HPP_CATEDU_GUI_GAME_GAME

#include "catedu/input.hpp"
#include "catedu/staging/ui/ui_user.hpp"
#include "catedu/ui/rendering/core.hpp"
#include "catedu/ui/rendering/font.hpp"
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

#endif // HPP_CATEDU_GUI_GAME_GAME
