///
/// BRIEF: Entry point for everything
///

#pragma once
#include "catedu/core/math/math.hpp"
#include "catedu/gui/editor/editor.hpp"
#include "catedu/gui/game/game.hpp"
#include "catedu/gui/main_menu/main_menu.hpp"
#include "catedu/rendering/3d/boxdraw.hpp"
#include "catedu/rendering/resource_spec.hpp"
#include "catedu/scene/scene.hpp"
#include "catedu/sys/input.hpp"
#include "catedu/sys/sokol_setup.hpp"
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_glue.h"
#include "sokol/sokol_log.h"

enum
{
    MENU_DEBUG,
    MENU_MAIN_MENU,
    MENU_EDITOR,
    MENU_GAME,
};

struct Entry : SokolSetup
{
    UiState ui_state;

    int ui_mode = MENU_EDITOR;
    GuiMainMenu main_menu;
    GuiEditor editor;
    GuiGame game_gui;
    ResourceSpec res;

    Scene scene;

    BoxdrawRenderer boxdraw_renderer;
    Input input_state;

    void init();
    void frame();
    void cleanup();
    void input(const sapp_event *event);
};
