#pragma once
#include "obj_tilemap.hpp"
#include "catedu/core/memory/buffer.hpp"
#include "catedu/rendering/3d/boxdraw.hpp"
#include "catedu/rendering/resource_spec.hpp"

struct LegacyScene {
    ObjTilemap ground;
    ObjTilemap middle;

    static LegacyScene init(Buffer file);
    void render(BoxdrawRenderer &renderer, ResourceSpec &resources);
    void deinit();
};
