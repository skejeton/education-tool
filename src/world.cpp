#include "world.hpp"
#include "boxdraw.hpp"
#include <cassert>
#include "math.hpp"
#include "memory.h"

void Building::render(BoxdrawRenderer *renderer) {
  Building building = *this;
  Vector2 position = { (float)building.x, (float)building.y };
  float building_height = building.height();
  Box3 box = building.box();

  switch (building.type) {
  case BuildingType::APARTMENT:
    boxdraw_push(renderer, boxdraw_cmdgradient(box, { 0.5, 0.5, 0.5, 1.0 }, { 0.4, 0.3, 0.3, 1.0 }));
    for (int t = 0; t < building.h; t++) {
      for (int x = -1; x <= 1; x++) {
        float y = t * 2 + 2;
        Box3 window = box3_extrude_from_point( { position.x+x*2.0f, y, position.y }, { 0.5, 0.75, 4.05 });
        boxdraw_push(renderer, boxdraw_cmdgradient(window, { 0.0, 0.0, 1.0, 1.0 }, { 1.0, 0.0, 0.1, 1.0 }));
      }
      for (int x = -1; x <= 1; x++) {
        float y = t * 2 + 2;
        Box3 window = box3_extrude_from_point( { position.x, y, position.y+x*2.0f }, { 4.05, 0.75, 0.5 });
        boxdraw_push(renderer, boxdraw_cmdgradient(window, { 0.0, 0.0, 1.0, 1.0 }, { 1.0, 0.0, 0.1, 1.0 }));
      }
    }
    break;
  case BuildingType::TREE:
    boxdraw_push(renderer, boxdraw_cmdgradient(box3_extrude_from_point({position.x, building_height/2, position.y}, {0.5, building_height/2, 0.5}), { 0.5, 0.2, 0.0, 1.0 }, { 0.6, 0.1, 0.0, 1.0 }));
    boxdraw_push(renderer, boxdraw_cmdgradient(box3_extrude_from_point({position.x, building_height+1, position.y}, {4, 2, 4}), { 0.0, 0.5, 0.2, 1.0 }, { 0.0, 0.6, 0.1, 1.0 }));
    break;
  case BuildingType::PAVEMENT:
    boxdraw_push(renderer, boxdraw_cmdgradient(box, { 0.5, 0.5, 0.5, 1.0 }, { 0.5, 0.5, 0.5, 1.0 }));
    break;
  }

}

void World::add_building(Building building) {
  assert(this->buildings_count < 256 && "Out of buildings");
  this->buildings[this->buildings_count++] = building;
}

void World::remove_building(size_t index) {
  assert(index < this->buildings_count);

  memmove(this->buildings+index, this->buildings+index+1, sizeof(Building) * (this->buildings_count-index-1));
  this->buildings_count -= 1;
}

