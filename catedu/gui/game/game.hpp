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
    char dialog_text[128];
    char dialog_ans[128];
    char* ans_out;
    bool dialog_open;
    bool dialog_has_ans;

    static GuiGame init(UiState* ui_state);
    void deinit();

    void show_dialogue(const char* text,
                       const char* ans = NULL,
                       char* ans_out = NULL);

    int show();
};

#endif // HPP_CATEDU_GUI_GAME_GAME
