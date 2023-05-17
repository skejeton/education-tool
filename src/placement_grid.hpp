///
/// BRIEF: Placement grid for world editing.
///

#ifndef H_PLACEMENT_GRID_CATEDU
#define H_PLACEMENT_GRID_CATEDU

#include "boxdraw.hpp"

enum PlacementGridTile {
  PLACEMENT_GRID_TILE_EMPTY,
  PLACEMENT_GRID_TILE_OCCUPIED,
  PLACEMENT_GRID_TILE_CANTPLACE,
  PLACEMENT_GRID_TILE_CANPLACE,
};

struct PlacementRegion {
  int x, y, w, h;
};

struct PlacementGrid {
  int offset_x, offset_y;
  PlacementGridTile tiles[160][160];

  static PlacementGrid init(int x, int y) {
    return {x, y};
  }

  void place_region(PlacementRegion region);
  bool can_place_region(PlacementRegion region);
  bool try_place_region(PlacementRegion region);
  void render(BoxdrawRenderer *boxdraw);
};



#endif