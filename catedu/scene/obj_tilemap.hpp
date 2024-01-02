#pragma once
#include "basic_tilemap.hpp"
#include "catedu/rendering/resource_spec.hpp"
#include "catedu/rendering/3d/boxdraw.hpp"

struct ObjTilemap
{
    BasicTilemap tilemap;

    static ObjTilemap init();
    void deinit();

    void set_tile(Vector2i tile_position, int tile_id);
    int get_tile(Vector2i tile_position);

    void render(BoxdrawRenderer &renderer, ResourceSpec &resources);
};
