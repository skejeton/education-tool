#pragma once

#include "catedu/genobj/render.hpp"
#include "catedu/gui/editor/dispatcher.hpp"
#include "catedu/ui/user.hpp"

struct EditDelete
{
    void show(UiPass &user, Renderer &renderer, Dispatcher &disp,
              GenResources &gen_resources, Input &input, Camera &camera);
};
