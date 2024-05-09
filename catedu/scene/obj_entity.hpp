#pragma once

#include "catedu/core/alloc/bump_allocator.hpp"
#include "catedu/core/math/math.hpp"
#include "catedu/rendering/3d/pbr.hpp"
#include "catedu/rendering/resource_spec.hpp"
#include "physics.hpp"

struct Dialog
{
    char name[64];
    char text[1024];
};

struct ObjEntity
{
    char model_name[32];
    Vector2 pos;
    TableId body_id;

    static ObjEntity init(const char *model_name, Vector2 pos);
    void deinit();
    void update(PhysicsWorld &world, ResourceSpec &resources);
    void render(catedu::pbr::Renderer &renderer, ResourceSpec &resources);

    ObjEntity copy();

    static ObjEntity load(void **data);
    void save(BumpAllocator &alloc);
};
