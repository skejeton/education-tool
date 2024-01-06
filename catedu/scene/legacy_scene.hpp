#pragma once
#include "obj_tilemap.hpp"
#include "obj_backdrop.hpp"
#include "catedu/core/memory/buffer.hpp"
#include "catedu/rendering/3d/boxdraw.hpp"
#include "catedu/rendering/resource_spec.hpp"
#include "obj_entity.hpp"

struct LegacyScene {
    ObjTilemap ground;
    ObjTilemap middle;
    ObjBackdrop backdrop;
    ObjEntity entities[64];
    size_t entity_count;

    static LegacyScene init(Buffer file);
    void render(BoxdrawRenderer &renderer, ResourceSpec &resources);
    void deinit();
};
