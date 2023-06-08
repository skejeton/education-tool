///
/// BRIEF: The world state.
/// 

#ifndef H_WORLD_CATEDU
#define H_WORLD_CATEDU

#include "boxdraw.hpp"
#include "scene.hpp"
#include "camera.hpp"
#include "placement_grid.hpp"
#include <stdlib.h>

#define WORLD_BUILDINGS_MAX 256

enum struct BuildingType {
  APARTMENT,
  PAVEMENT,
  TREE
};

struct Building {
  int x, y, h;

  BuildingType type;

  inline float height() {
    return h * 2 + 2;
  }

  inline PlacementRegion region() {
    switch (type) {
    case BuildingType::APARTMENT:
    case BuildingType::TREE:
      return {x-4, y-4, 8, 8};
    case BuildingType::PAVEMENT:
      return {x-2, y-2, 4, 4};
    }
    abort();
  }

  inline Rect collision_rect() {
    switch (type) {
    case BuildingType::APARTMENT:
      return {(float)x-4, (float)y-4, 8, 8};
    case BuildingType::TREE:
      return {(float)x-0.5f, (float)y-0.5f, 1, 1};
    case BuildingType::PAVEMENT:
      return {};
    }
    abort();
  }

  inline Box3 box() {
    float building_height = height();
    switch (type) {
    case BuildingType::APARTMENT:
      return box3_extrude_from_point({ (float)x, building_height/2, (float)y }, { 4, building_height/2, 4 });
    case BuildingType::TREE:
      return box3_extrude_from_point({ (float)x, building_height/2, (float)y }, { 4, building_height/2, 4 });
    case BuildingType::PAVEMENT:
      return box3_extrude_from_point({ (float)x, 0.0, (float)y }, { 2, 0.2, 2 });
    }
    abort();
  }

  void render(BoxdrawRenderer *renderer);
};

struct World {
  Scene entities;
  Building buildings[WORLD_BUILDINGS_MAX];
  size_t buildings_count;

  void add_building(Building building);
  void remove_building(size_t index);
};

#endif
