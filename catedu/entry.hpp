///
/// BRIEF: Entry point for everything
///

#pragma once
#include "catedu/gui/editor/editor.hpp"
#include "catedu/gui/main_menu/main_menu.hpp"
#include "catedu/gui/transition/transition.hpp"
#include "catedu/rendering/resource_spec.hpp"
#include "catedu/sys/input.hpp"
#include "catedu/sys/sokol_setup.hpp"
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_glue.h"
#include "sokol/sokol_log.h"
#include <umka_api.h>

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
    UiUser *ui_user;

#ifdef RUNTIME_MODE
    int ui_mode = MENU_EDITOR;
#else
    int ui_mode = MENU_MAIN_MENU;
#endif
    GuiMainMenu main_menu;
    GuiEditor editor;
    GuiTransition transition;
    ResourceSpec res;

    void *umka;

    catedu::pbr::Renderer renderer;

    void init();
    void frame();
    void cleanup();
    void input(const sapp_event *event);
};
