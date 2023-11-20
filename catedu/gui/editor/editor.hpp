#ifndef HPP_CATEDU_GUI_EDITOR_EDITOR
#define HPP_CATEDU_GUI_EDITOR_EDITOR

#include "catedu/input.hpp"
#include "catedu/staging/ui/ui_user.hpp"
#include "catedu/ui/rendering/core.hpp"
#include "catedu/ui/rendering/font.hpp"
#include <sokol/sokol_app.h>

struct GuiEditor
{
    UiState* ui_state;
    int selection = 0;

    static GuiEditor init(UiState* ui_state);
    void deinit();

    int show(Vector2 campos);
};

#endif // HPP_CATEDU_GUI_EDITOR_EDITOR
