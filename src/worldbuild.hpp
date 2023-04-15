///
/// BRIEF: World building
///
/*
#ifndef H_WORLDBUILD_CATEDU
#define H_WORLDBUILD_CATEDU

#include "boxdraw.hpp"

struct BuildingDesc {
  int height;
  int x, y;
}

struct WorldData {
  BuildingDesc buildings[32];
  size_t buildings_count;
}

void wb_render_world(WorldData *wd, BoxdrawRenderer *ren);


#endif // H_WORLDBUILD_CATEDU

*/