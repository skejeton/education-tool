///
/// BRIEF: Placement grid for world editing.
///

#ifndef H_PLACEMENT_GRID_CATEDU
#define H_PLACEMENT_GRID_CATEDU
#include "boxdraw.hpp"
#include "stadef.h"

enum PlacementGridTile
{
    PLACEMENT_GRID_TILE_EMPTY,
    PLACEMENT_GRID_TILE_OCCUPIED,
    PLACEMENT_GRID_TILE_CANTPLACE,
    PLACEMENT_GRID_TILE_CANPLACE,
};

struct PlacementRegion
{
    int x, y, w, h;
};

struct PlacementGrid
{
    int offset_x, offset_y;
    PlacementGridTile tiles[160][160];

    static PlacementGrid init(int x, int y) { return { x, y }; }

    float map_ray(Ray3 ray, int* x, int* y)
    {
        float ground_t;
        if (ray3_vs_horizontal_plane(ray, 0, &ground_t)) {
            Vector3 at = ray3_at(ray, ground_t);
            *x = at.x;
            *y = at.z;
            return ground_t;
        }
        return DEF_EPSILON_MAX;
    }

    void place_region(PlacementRegion region);
    bool can_place_region(PlacementRegion region);
    bool try_place_region(PlacementRegion region);
    void render(BoxdrawRenderer* boxdraw);
};

#endif
