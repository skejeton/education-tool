#pragma once
#include "catedu/gui/editor/camera.hpp"
#include "catedu/gui/transition/transition.hpp"
#include "catedu/rendering/3d/pbr.hpp"
#include "catedu/rendering/resource_spec.hpp"
#include "catedu/scene/world.hpp"
#include "catedu/ui/user.hpp"
#include <sokol/sokol_app.h>

struct GuiMainMenu
{
    enum
    {
        normal,
        settings,
        exit,
    } state;
    UiState *ui_state;

    float angle = 0;
    EditorCamera camera;

    static GuiMainMenu init(UiState *ui_state);
    void deinit();

    // @returns true if the menu switched to the editor.
    bool show(UiPass &user, GuiTransition &transition, World &world,
              catedu::pbr::Renderer &renderer, ResourceSpec &resources);
};
