#pragma once

#include "catedu/math.hpp"
#include "catedu/rendering/3d/boxdraw.hpp"
#include "catedu/rendering/resource_spec.hpp"

struct ObjEntity
{
    char model_name[32];
    Vector2 pos;

    static ObjEntity init(const char *model_name, Vector2 pos);
    void deinit();
    void render(BoxdrawRenderer &renderer, ResourceSpec &resources);
};
