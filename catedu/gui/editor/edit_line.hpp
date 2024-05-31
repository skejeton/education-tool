///
/// NOTE: For editing walls and roads
///

#pragma once

#include "catedu/genobj/render.hpp"
#include "catedu/gui/editor/dispatcher.hpp"
#include "catedu/rendering/3d/camera.hpp"
#include "catedu/ui/user.hpp"

struct EditLine
{
    bool started;
    Vector2 pointer_start;
    Vector2 pointer_end;

    void show(UiUser &user, Object::Type type, catedu::pbr::Renderer &renderer,
              Dispatcher &disp, GenResources &gen_resources, Input &input,
              Camera &camera);
};
