#pragma once
#include "../debugtree/debugtree.hpp"
#include "catedu/rendering/3d/camera.hpp"
#include "catedu/rendering/3d/pbr.hpp"
#include "catedu/rendering/resource_spec.hpp"
#include "catedu/ui/user.hpp"
#include "world.hpp"
#include <sokol/sokol_app.h>

enum EditorTab
{
    EDITOR_TAB_CONFIG,
    EDITOR_TAB_BUILD,
    EDITOR_TAB_CHARACTERS,
    EDITOR_TAB_SCRIPT
};

struct GuiEditor
{
    bool dirty;
    bool exit_requested;
    bool tried_to_return_back;
    bool show_debug;
    UiState *ui_state;
    Camera camera;
    GuiDebugTree debug_tree;
    EditorTab tab;
    World world;

    static GuiEditor init(UiState *ui_state);

    bool show(catedu::pbr::Renderer &renderer, ResourceSpec &resources,
              UiUser &user);
    void deinit();
};
