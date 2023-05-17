#include "world.hpp"
#include "boxdraw.hpp"

#if 0
void render_building(BoxdrawRenderer *boxdraw, Building building) {
  Vector2 position = { (float)building.x, (float)building.y };
  float building_height = building.height();
  Box3 box = building.box();
  boxdraw_push(boxdraw, boxdraw_cmdgradient(box, { 0.5, 0.5, 0.5, 1.0 }, { 0.4, 0.3, 0.3, 1.0 }));
  for (int t = 0; t < building.h; t++) {
    for (int x = -1; x <= 1; x++) {
      float y = t * 2 + 2;
      Box3 window = box3_extrude_from_point( { position.x+x*2.0f, y, position.y }, { 0.5, 0.75, 4.05 });
      boxdraw_push(boxdraw, boxdraw_cmdgradient(window, { 0.0, 0.0, 1.0, 1.0 }, { 1.0, 0.0, 0.1, 1.0 }));
    }
    for (int x = -1; x <= 1; x++) {
      float y = t * 2 + 2;
      Box3 window = box3_extrude_from_point( { position.x, y, position.y+x*2.0f }, { 4.05, 0.75, 0.5 });
      boxdraw_push(boxdraw, boxdraw_cmdgradient(window, { 0.0, 0.0, 1.0, 1.0 }, { 1.0, 0.0, 0.1, 1.0 }));
    }
  }
}
#endif