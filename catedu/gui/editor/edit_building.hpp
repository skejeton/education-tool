#pragma once
#include "world.hpp"
#include <catedu/genobj/render.hpp>
#include <catedu/rendering/3d/camera.hpp>
#include <catedu/ui/user.hpp>

struct EditBuilding
{
    bool placing;
    int x;
    int y;
    int floors;

    void show(UiUser &user, catedu::pbr::Renderer &renderer, World &world,
              GenResources &gen_resources, Input &input, Camera &camera);
};
