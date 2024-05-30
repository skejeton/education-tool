#pragma once
#include "../debugtree/debugtree.hpp"
#include "catedu/gui/editor/camera.hpp"
#include "catedu/gui/editor/dispatcher.hpp"
#include "catedu/gui/editor/edit_building.hpp"
#include "catedu/rendering/3d/camera.hpp"
#include "catedu/rendering/3d/pbr.hpp"
#include "catedu/rendering/resource_spec.hpp"
#include "catedu/ui/user.hpp"
#include "edit_basic.hpp"
#include "playtest.hpp"
#include <sokol/sokol_app.h>

struct SubEditor
{
    enum class Type
    {
        Building,
        Road,
        Player,
        Wall
    } type;

    EditBasic edit_basic;
    EditBuilding edit_building;

    void show(UiUser &user, catedu::pbr::Renderer &renderer, Dispatcher &disp,
              GenResources &gen_resources, Input &input, Camera &camera);
};

struct GuiEditor
{
    bool exit_requested;
    bool tried_to_return_back;
    bool playtest_no_player;
    bool show_debug;
    bool playtesting;
    Playtest playtest;
    UiState *ui_state;
    EditorCamera editor_camera;
    GuiDebugTree debug_tree;
    SubEditor sub_editor;
    Dispatcher dispatcher;

    static GuiEditor init(UiState *ui_state);

    bool show(catedu::pbr::Renderer &renderer, ResourceSpec &resources,
              UiUser &user);
    void deinit();
};
