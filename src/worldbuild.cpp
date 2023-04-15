#include "worldbuild.hpp"
/*
static void render_building(BoxdrawRenderer *ren, BuildingDesc building) {
  float building_height = bheight * 4 + 4;
  Box3 building_box = box3_extrude_from_point({ 0, building_height/2, 0 }, { 8, building_height/2, 8 });
  boxdraw_push(ren, boxdraw_cmdgradient(building_box, { 0.5, 0.5, 0.5, 1.0 }, { 0.4, 0.3, 0.3, 1.0 }));

  for (int t = 0; t < building.height; t++) {
    for (int x = -1; x <= 1; x++) {
      float y = t*4 + 4;
      Box3 window = box3_extrude_from_point({ x*4.0f, y, 0 }, { 1, 1.5, 8.1 });
      boxdraw_push(ren, boxdraw_cmdgradient(window, { 0.0, 0.0, 1.0, 1.0 }, { 1.0, 0.0, 0.1, 1.0 }));
    }

    for (int x = -1; x <= 1; x++) {
      float y = t*4 + 4;
      Box3 window = box3_extrude_from_point({ 0, y, x*4.0f }, { 8.1, 1.5, 1 });
      boxdraw_push(ren, boxdraw_cmdgradient(window, { 0.0, 0.0, 1.0, 1.0 }, { 1.0, 0.0, 0.1, 1.0 }));
    }
  }  
}



void wb_render_world(WorldData *wd, BoxdrawRenderer *ren) {
  Box3 floor = box3_extrude_from_point({ 0, 0, 0 }, { 50, 0.1, 50 });
  boxdraw_push(ren, boxdraw_cmdcolored(floor, { 0.6, 0.6, 0.3, 1.0 }));
 
  
} 
*/