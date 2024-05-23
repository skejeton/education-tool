#pragma once
#include "../debugtree/debugtree.hpp"
#include "catedu/gui/editor/edit_building.hpp"
#include "catedu/rendering/3d/camera.hpp"
#include "catedu/rendering/3d/pbr.hpp"
#include "catedu/rendering/resource_spec.hpp"
#include "catedu/ui/user.hpp"
#include "edit_road.hpp"
#include "world.hpp"
#include <sokol/sokol_app.h>

struct SubEditor
{
    enum class Type
    {
        Building,
        Road,
    } type;

    EditRoad edit_road;
    EditBuilding edit_building;

    void show(UiUser &user, catedu::pbr::Renderer &renderer, World &world,
              GenResources &gen_resources, Input &input, Camera &camera);
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
    World world;

    SubEditor sub_editor;

    static GuiEditor init(UiState *ui_state);

    bool show(catedu::pbr::Renderer &renderer, ResourceSpec &resources,
              UiUser &user);
    void deinit();
};
