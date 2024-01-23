#pragma once

#include "catedu/core/math/math.hpp"
#include "catedu/rendering/3d/boxdraw.hpp"
#include "catedu/rendering/resource_spec.hpp"
#include "physics.hpp"

struct ObjEntity
{
    char model_name[32];
    Vector2 pos;
    TableId body_id;

    static ObjEntity init(const char *model_name, Vector2 pos);
    void deinit();
    void update(PhysicsWorld &world, ResourceSpec &resources);
    void render(BoxdrawRenderer &renderer, ResourceSpec &resources);
};
