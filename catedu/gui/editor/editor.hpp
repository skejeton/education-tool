#pragma once
#include "catedu/rendering/3d/boxdraw.hpp"
#include "catedu/rendering/3d/camera.hpp"
#include "catedu/rendering/resource_spec.hpp"
#include "catedu/scene/scene.hpp"
#include "catedu/sys/input.hpp"
#include "catedu/ui/rendering/core.hpp"
#include "catedu/ui/rendering/font.hpp"
#include "catedu/ui/user.hpp"
#include <sokol/sokol_app.h>

struct GuiEditor
{
    UiState *ui_state;
    TableId selection;
    bool create_entity;
    size_t entity_list_page;
    Camera camera;

    static GuiEditor init(UiState *ui_state);
    void show(BoxdrawRenderer &renderer, ResourceSpec &resources, Scene &scene,
              Input &input);
    void deinit();
};
