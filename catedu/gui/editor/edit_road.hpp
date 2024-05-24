#pragma once
#include "catedu/gui/editor/dispatcher.hpp"
#include <catedu/genobj/render.hpp>
#include <catedu/rendering/3d/camera.hpp>
#include <catedu/ui/user.hpp>

struct EditRoad
{
    void show(UiUser &user, catedu::pbr::Renderer &renderer, Dispatcher &disp,
              GenResources &gen_resources, Input &input, Camera &camera);
};
