#pragma once
#include "basic_tilemap.hpp"
#include "catedu/core/math/math.hpp"

struct Space
{
    BasicTilemap claimed_tiles;

    static Space create();
    void destroy();

    void claim_region_rect(RectI region);
    void unclaim_region_rect(RectI region);
    bool is_region_claimed(RectI region);
};
