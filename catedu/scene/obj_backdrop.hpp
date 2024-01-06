#pragma once

#include "catedu/math.hpp"
#include "catedu/rendering/3d/boxdraw.hpp"
#include "catedu/rendering/resource_spec.hpp"

struct ObjBackdrop
{
    Rect texture_rect;

    static ObjBackdrop init(Rect texture_rect);
    void deinit();
    void render(BoxdrawRenderer &renderer, ResourceSpec &resources);
};
