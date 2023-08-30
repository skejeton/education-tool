#include "placement_grid.hpp"

static const Vector4 placement_colorscheme[4] = {
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 1.0f, 0.0f, 0.5f, 1.0f },
    { 1.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 1.0f, 0.0f, 1.0f },
};

static void
render_grid(BoxdrawRenderer* boxdraw, PlacementGrid* grid)
{
    Vector4 grid_color = { 1.0, 1.0, 0.0, 1.0 };

    float offset_x = grid->offset_x;
    float offset_y = grid->offset_y;

    for (int i = -80; i <= 80; i++) {
        Box3 box = box3_extrude_from_point(
          { (float)i + offset_x, 0.0f, offset_y }, { 0.05f, 0.15f, 80.0f });
        boxdraw_push(boxdraw, boxdraw_cmdcolored(box, grid_color));
    }
    for (int i = -80; i <= 80; i++) {
        Box3 box = box3_extrude_from_point(
          { offset_x, 0.0f, (float)i + offset_y }, { 80.0f, 0.15f, 0.05f });
        boxdraw_push(boxdraw, boxdraw_cmdcolored(box, grid_color));
    }
    for (int i = 0; i < 160; i++) {
        for (int j = 0; j < 160; j++) {
            if (grid->tiles[i][j]) {
                int x = (i - 80) + offset_x;
                int y = (j - 80) + offset_y;
                Box3 box = box3_extrude_from_point(
                  { (float)x + 0.5f, 0.0f, (float)y + 0.5f },
                  { 0.5f, 0.13f, 0.5f });
                boxdraw_push(boxdraw,
                             boxdraw_cmdcolored(
                               box, placement_colorscheme[grid->tiles[i][j]]));
            }
        }
    }
}

static bool
can_place_tile(PlacementGrid* grid, int x, int y)
{
    x += -grid->offset_x + 80;
    y += -grid->offset_y + 80;

    if (x >= 0 && y >= 0 && x < 160 && y < 160) {
        return grid->tiles[x][y] == PLACEMENT_GRID_TILE_EMPTY;
    }

    return false;
}

static void
place_tile(PlacementGrid* grid, int x, int y, PlacementGridTile type)
{
    x += -grid->offset_x + 80;
    y += -grid->offset_y + 80;
    if (x >= 0 && y >= 0 && x < 160 && y < 160) {
        grid->tiles[x][y] = type;
    }
}

void
PlacementGrid::place_region(PlacementRegion region)
{
    for (int x = 0; x < region.w; x++) {
        for (int y = 0; y < region.h; y++) {
            place_tile(
              this, x + region.x, y + region.y, PLACEMENT_GRID_TILE_OCCUPIED);
        }
    }
}

bool
PlacementGrid::can_place_region(PlacementRegion region)
{
    for (int x = 0; x < region.w; x++) {
        for (int y = 0; y < region.h; y++) {
            if (!can_place_tile(this, x + region.x, y + region.y)) {
                return false;
            }
        }
    }

    return true;
}

bool
PlacementGrid::try_place_region(PlacementRegion region)
{
    bool can_place = this->can_place_region(region);
    PlacementGridTile tile_type =
      can_place ? PLACEMENT_GRID_TILE_CANPLACE : PLACEMENT_GRID_TILE_CANTPLACE;

    for (int x = 0; x < region.w; x++) {
        for (int y = 0; y < region.h; y++) {
            place_tile(this, x + region.x, y + region.y, tile_type);
        }
    }

    return can_place;
}

void
PlacementGrid::render(BoxdrawRenderer* boxdraw)
{
    render_grid(boxdraw, this);
}
