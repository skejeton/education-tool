#ifndef HPP_CATEDU_GUI_GAME_GAME
#define HPP_CATEDU_GUI_GAME_GAME

#include "catedu/input.hpp"
#include "catedu/staging/ui/ui_user.hpp"
#include "catedu/ui/rendering/core.hpp"
#include "catedu/ui/rendering/font.hpp"
#include <sokol/sokol_app.h>

struct GuiGame
{
    UiState* ui_state;
    bool dialog_open;

    static GuiGame init(UiState* ui_state);
    void deinit();

    int show();
};

#endif // HPP_CATEDU_GUI_GAME_GAME
