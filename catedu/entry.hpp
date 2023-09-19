///
/// BRIEF: Entry point for everything
///

#ifndef H_CATEDU_ENTRY
#define H_CATEDU_ENTRY

#include "boxdraw.hpp"
#include "camera.hpp"
#include "camera_input.hpp"
#include "console.hpp"
#include "dialog.hpp"
#include "entity_editor.hpp"
#include "environment.hpp"
#include "flashbacks.hpp"
#include "gui/main_menu/main_menu.hpp"
#include "help_menu.hpp"
#include "imgui/imgui.h"
#include "input.hpp"
#include "main_menu.hpp"
#include "math.hpp"
#include "netcode.hpp"
#include "placement_grid.hpp"
#include "player.hpp"
#include "pointing_at_resolve.hpp"
#include "rpc.hpp"
#include "scene.hpp"
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_glue.h"
#include "sokol/sokol_log.h"
#include "sokol/util/sokol_imgui.h"
#include "temp_id_binder.hpp"
#include "ui/rendering/font.hpp"
#include "ui/rendering/pass.hpp"
#include <memory>
#include <stdio.h>

enum OpenMenu
{
    HELP,
    DIALOG_EDITOR
};

enum class SelectionOption
{
    ENTITY,
    BUILDING,
    PAVEMENT,
    TREE,
    COUNT
};

struct Entry
{
    Input inputs;
    Environment env;
    Netcode nc;

    FlashbacksGui flashbacks_gui;
    BoxdrawRenderer boxdraw;
    int bheight = 0;
    int cmdc = 0;
    bool show_objects = true;
    TableId last_object_locator = {};
    TableId entity_selected;
    EntityEditor entity_editor;
    SelectionOption selection_option;
    ImFont* main_font;
    HelpMenu help_menu;
    MainMenu main_menu;
    OpenProject open_project;
    OpenMenu menu;
    DialogQueue dialog_queue;
    GuiMainMenu main_menu_b;

    void init();
    void input(const sapp_event* event);
    void cleanup();
    void frame();
};

#endif // H_CATEDU_ENTRY
