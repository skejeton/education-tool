///
/// BRIEF: The world state.
/// 

#ifndef H_WORLD_CATEDU
#define H_WORLD_CATEDU

#include "boxdraw.hpp"
#include "scene.hpp"

struct Building {
  int x, y, h;

  inline float height() {
    return h * 2 + 2;
  }

  inline Box3 box() {
    float building_height = height();
    return box3_extrude_from_point({ (float)x, building_height/2, (float)y }, { 4, building_height/2, 4 });
  }
};



struct World {
  Scene entities;
  Building buildings[256];
  size_t buildings_count;

  void render(BoxdrawRenderer *renderer);
};

struct WorldPlayer {
  World *world;

  void render(BoxdrawRenderer *renderer);
};

struct WorldEditor {
  World *world;

  void render(BoxdrawRenderer *renderer);
};

#endif
