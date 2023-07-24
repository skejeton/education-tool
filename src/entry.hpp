///
/// BRIEF: Entry point for everything
///

#ifndef H_CATEDU_ENTRY
#define H_CATEDU_ENTRY

#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_log.h"
#include "sokol/sokol_glue.h"
#include "imgui/imgui.h"
#include "sokol/util/sokol_imgui.h"
#include "math.hpp"
#include "console.hpp"
#include "camera.hpp"
#include "input.hpp"
#include "camera_input.hpp"
#include "scene.hpp"
#include "flashbacks.hpp"
#include "boxdraw.hpp"
#include "pointing_at_resolve.hpp"
#include "temp_id_binder.hpp"
#include "placement_grid.hpp"
#include "help_menu.hpp"
#include <memory>
#include <stdio.h>
#include "main_menu.hpp"
#include "entity_editor.hpp"
#include "rpc.hpp"
#include "dialog.hpp"
#include "player.hpp"
#include "environment.hpp"
#include "netcode.hpp"

enum OpenMenu {
    HELP,
    DIALOG_EDITOR
};

enum class SelectionOption {
    ENTITY,
    BUILDING,
    PAVEMENT,
    TREE,
    COUNT
};

struct Entry {
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
    ImFont *main_font;
    HelpMenu help_menu;
    MainMenu main_menu;
    OpenProject open_project;
    OpenMenu menu;
    DialogQueue dialog_queue;

    void init();
    void input(const sapp_event* event);
    void cleanup();
    void frame();
};

#endif // H_CATEDU_ENTRY
