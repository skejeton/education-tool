#pragma once

#include "catedu/core/math/math.hpp"
#include "catedu/rendering/3d/boxdraw.hpp"
#include "catedu/rendering/resource_spec.hpp"
#include "physics.hpp"

struct ObjBackdrop
{
    Rect texture_rect;

    static ObjBackdrop init(Rect texture_rect);
    void deinit();
    void update(PhysicsWorld &world, ResourceSpec &resources);
    void render(BoxdrawRenderer &renderer, ResourceSpec &resources);

    ObjBackdrop copy();
};
