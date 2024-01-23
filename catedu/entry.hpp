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

struct Entry : SokolSetup
{
    UiState ui_state;
    Scene scene;
    GuiMainMenu main_menu;
    GuiEditor editor;
    GuiGame game_gui;
    ResourceSpec res;
    int ui_mode = 2;
    BoxdrawRenderer boxdraw_renderer;
    Vector2 target_camera_pos;
    bool zoomout = false;
    Input input_state;
    int floor;

    void init();
    void frame();
    void cleanup();
    void input(const sapp_event *event);
};
