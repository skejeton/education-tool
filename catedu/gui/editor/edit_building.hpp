#pragma once
#include "catedu/genobj/render.hpp"
#include "catedu/gui/editor/dispatcher.hpp"
#include "catedu/rendering/3d/camera.hpp"
#include "catedu/ui/user.hpp"

struct EditBuilding
{
    bool placing;
    float x;
    float y;
    int floors;

    void show(UiPass &user, Renderer &renderer, Dispatcher &disp,
              GenResources &gen_resources, Input &input, Camera &camera);
};
