///
/// BRIEF: Entry point for everything
///

#ifndef H_CATEDU_ENTRY
#define H_CATEDU_ENTRY

#include "boxdraw.hpp"
#include "gui/editor/editor.hpp"
#include "gui/game/game.hpp"
#include "gui/main_menu/main_menu.hpp"
#include "input.hpp"
#include "math.hpp"
#include "physics.hpp"
#include "scripts.hpp"
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_glue.h"
#include "sokol/sokol_log.h"
#include "staging/world/resource_spec.hpp"
#include "staging/world/world.hpp"

struct Entry
{
    WorldState world_state;
    UiState ui_state;
    GuiMainMenu main_menu;
    GuiEditor editor;
    GuiGame game_gui;
    ResourceSpec res;
    World world;
    int ui_mode;
    BoxdrawRenderer boxdraw_renderer;
    Vector2 target_camera_pos;
    bool zoomout = false;
    ScriptData script_data;
    Input input_state;
    int floor;

    void init();
    void input(const sapp_event *event);
    void cleanup();
    void frame();
};

#endif // H_CATEDU_ENTRY
