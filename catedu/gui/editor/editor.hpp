#pragma once
#include "catedu/gui/editor/camera.hpp"
#include "catedu/gui/editor/dispatcher.hpp"
#include "catedu/gui/editor/edit_building.hpp"
#include "catedu/gui/editor/edit_delete.hpp"
#include "catedu/gui/editor/edit_line.hpp"
#include "catedu/gui/transition/transition.hpp"
#include "catedu/rendering/3d/camera.hpp"
#include "catedu/rendering/3d/pbr.hpp"
#include "catedu/rendering/resource_spec.hpp"
#include "catedu/ui/user.hpp"
#include "edit_basic.hpp"
#include "playtest.hpp"
#include "script.hpp"
#include <sokol/sokol_app.h>

enum class SubMode
{
    Build,
    Character,
    Script
};

struct SubEditor
{
    enum class Type
    {
        Deleter,
        Building,
        Road,
        Player,
        Wall,
        Tree
    } type;

    EditDelete edit_delete;
    EditBasic edit_basic;
    EditBuilding edit_building;
    EditLine edit_line;

    void show(UiPass &user, Renderer &renderer, Dispatcher &disp,
              GenResources &gen_resources, Input &input, Camera &camera);
};

struct GuiEditor
{
    bool exit_requested;
    bool tried_to_return_back;
    bool playtest_no_player;
    bool playtesting;
    bool returning_back;
    Playtest playtest;
    UiState *ui_state;
    EditorCamera editor_camera;
    SubEditor sub_editor;
    SubMode sub_mode;
    Dispatcher dispatcher;
    Script script;

    // HACK: This is a hack to allow to set the camera position to the center of
    // the place, because the editor can't track when the place is changed.
    Place *previous_place;

    static GuiEditor init(UiState *ui_state);

    bool show(UiPass &user, GuiTransition &transition, Renderer &renderer,
              ResourceSpec &resources);
    void deinit();
};
