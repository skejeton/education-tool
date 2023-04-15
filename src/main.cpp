#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_log.h"
#include "sokol/sokol_glue.h"
#include "imgui/imgui.h"
#include "sokol/util/sokol_imgui.h"
#include "math.hpp"
#include "console.hpp"
#include "camera.hpp"
#include "input.hpp"
#include "camera_input.hpp"
#include "scene.hpp"
#include "flashbacks.hpp"
#include "boxdraw.hpp"
#include <stdio.h>

static Input inputs;
static Camera camera;
static Scene game_scene;
static Flashbacks flashbacks;
static FlashbacksGui flashbacks_gui;
static BoxdrawRenderer boxdraw;

static const FlashbacksDialogPrototype ent_1_dialogs[] = {
  { "Hello, there. I am here to teach you how to interact with this world.", nullptr },
  { "First off, you can move around with WASD, and press the mouse button to TALK. Like you've probably already figured out.", nullptr },
  { "After this discussion ends, you can press BACKLOG button, on the top left corner, to repeat it.", nullptr },
  { "Now let's try a simple exam. What controls would you use to move: ", "WASD and mouse button"},
  { "How would you re-try a specific question or information sheet?", "BACKLOG button"},
};

static const FlashbacksDialogPrototype ent_2_dialogs[] = {
  { "Hello I'm here to teach you SIMD.\nSIMD (Single Instruction - Multiple Data), is a programming approach where instead of performing an operation on one value, we perform operations on several values at once.", nullptr },
  { "It's like operating on arrays.\nFor example what will be [1, 2, 3, 4] + [1, 1, 1, 1]?", "[2, 3, 4, 5]"},
  { "We've just added 1 to each value on the left.\nNow why is this useful?\nWhile SIMD may not be convenient to write, it allows us to make CPU execute our code much faster when used in an appropriate place.", nullptr },
  { "Let's try multiplication: [1, 2, 3, 4] * [2, 2, 2, 2]", "[2, 4, 6, 8]" },
  { "It's as easy as this! The more trickier part is how to apply it.", nullptr },
};


void init(void) {
  console_create_or_bind_existing();
  
#if 0
  EntityId ent1 = scene_summon_entity(&game_scene, { { 0, 0, 1 } });
  EntityId ent2 = scene_summon_entity(&game_scene, { { -6, 0, 1 } });

  {
    FlashbacksDialogMaker maker = FlashbacksDialogMaker::from(&flashbacks);
  
    for (auto dialog : ent_1_dialogs) {
      maker.append_dialog(dialog);
    }

    scene_get_entity(&game_scene, ent1)->dialog_id = maker.starter_id;
  }
  {
    FlashbacksDialogMaker maker = FlashbacksDialogMaker::from(&flashbacks);
  
    for (auto dialog : ent_2_dialogs) {
      maker.append_dialog(dialog);
    }
    
    scene_get_entity(&game_scene, ent2)->dialog_id = maker.starter_id;
  }
#endif


  flashbacks_gui = FlashbacksGui::create(&flashbacks);


    
  // setup sokol-gfx, sokol-time and sokol-imgui
  sg_desc desc = {};
  desc.context = sapp_sgcontext();
  desc.logger.func = slog_func;
  sg_setup(&desc);

  // use sokol-imgui with all default-options (we're not doing
  // multi-sampled rendering or using non-default pixel formats)
  simgui_desc_t simgui_desc = {};
  simgui_setup(&simgui_desc);
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  boxdraw = boxdraw_create();

  camera = Camera::init(45);
  camera.move(10, 10, 10);
}

int bheight = 0;


struct Building {
  int x, y, h;
};

struct World {
  Building buildings[32];
  size_t buildings_count;
};

enum PlacementGridTile {
  PLACEMENT_GRID_TILE_EMPTY,
  PLACEMENT_GRID_TILE_OCCUPIED,
  PLACEMENT_GRID_TILE_CANTPLACE,
  PLACEMENT_GRID_TILE_CANPLACE,
};

struct PlacementGrid {
  PlacementGridTile tiles[32][32];
};

struct Placement {
  int height;
  int x, y;
};

Vector4 placement_colorscheme[4] = {
  { 0.0f, 0.0f, 0.0f, 0.0f },
  { 1.0f, 0.0f, 0.5f, 1.0f },
  { 1.0f, 0.0f, 0.0f, 1.0f },
  { 0.0f, 1.0f, 0.0f, 1.0f },
};

bool map_ray_to_grid(Ray3 ray, int *x, int *y) {
  float ground_t;
  if (ray3_vs_horizontal_plane(ray, 0, &ground_t)) {
    Vector3 at = ray3_at(ray, ground_t);
    *x = at.x+16;
    *y = at.z+16;
    return *x >= 0 && *x < 32 && *y >= 0 && *y < 32;
  }

  return false;
}

bool tile_available(PlacementGrid *grid, int x, int y) {
  return x >= 0 && x < 32 && y >= 0 && y < 32;
}

bool place_building(PlacementGrid *grid, int x, int y, bool placed) {
  if (placed) {
    for (int i = -4; i < 4; i++) {
      for (int j = -4; j < 4; j++) {
        if (tile_available(grid, i+x, j+y)) {
          grid->tiles[i+x][j+y] = PLACEMENT_GRID_TILE_OCCUPIED;
        }
      }
    }
  } else {
    bool can_place = true;
    for (int i = -4; i < 4; i++) {
      for (int j = -4; j < 4; j++) {
        if (!tile_available(grid, i+x, j+y) || grid->tiles[i+x][j+y] == PLACEMENT_GRID_TILE_OCCUPIED) {
          can_place = false;
          goto assign;
        }
      }
    }

    assign:
    for (int i = -4; i < 4; i++) {
      for (int j = -4; j < 4; j++) {
        if (tile_available(grid, i+x, j+y)) {
          grid->tiles[i+x][j+y] = can_place ? PLACEMENT_GRID_TILE_CANPLACE : PLACEMENT_GRID_TILE_CANTPLACE;
        }
      }
    }

    return can_place;
  }

  return false;
}

void world_place_building(World *world, Building building) {
  assert(world->buildings_count < 32 && "Out of buildings");
  world->buildings[world->buildings_count++] = building;
}

void render_grid(PlacementGrid *grid) {
  Vector4 grid_color = {1.0, 1.0, 0.0, 1.0};
  Vector4 occupied_color = {1.0, 0.0, 0.0, 1.0};

  for (int i = -16; i <= 16; i++) {
    Box3 box = box3_extrude_from_point( { (float)i, 0.0f, 0.0f }, { 0.05f, 0.15f, 16.0f });
    boxdraw_push(&boxdraw, boxdraw_cmdcolored(box, grid_color));
  }
  for (int i = -16; i <= 16; i++) {
    Box3 box = box3_extrude_from_point( { 0.0f, 0.0f, (float)i }, { 16.0f, 0.15f, 0.05f });
    boxdraw_push(&boxdraw, boxdraw_cmdcolored(box, grid_color));
  }
  for (int i = 0; i < 32; i++) {
    for (int j = 0; j < 32; j++) {
      if (grid->tiles[i][j]) {
        int x = (i - 16);
        int y = (j - 16);
        Box3 box = box3_extrude_from_point( { (float)x+0.5f, 0.0f, (float)y+0.5f }, { 0.5f, 0.13f, 0.5f });
        boxdraw_push(&boxdraw, boxdraw_cmdcolored(box, placement_colorscheme[grid->tiles[i][j]]));
      }
    }
  }
}

void render_building(int x, int y, int h) {
  Vector2 position = { (float)x, (float)y };
  float building_height = h * 2 + 2;
  Box3 building = box3_extrude_from_point({ position.x, building_height/2, position.y }, { 4, building_height/2, 4 });
  boxdraw_push(&boxdraw, boxdraw_cmdgradient(building, { 0.5, 0.5, 0.5, 1.0 }, { 0.4, 0.3, 0.3, 1.0 }));
  for (int t = 0; t < h; t++) {
    for (int x = -1; x <= 1; x++) {
      float y = t * 2 + 2;
      Box3 window = box3_extrude_from_point( { position.x+x*2.0f, y, position.y }, { 0.5, 0.75, 4.05 });
      boxdraw_push(&boxdraw, boxdraw_cmdgradient(window, { 0.0, 0.0, 1.0, 1.0 }, { 1.0, 0.0, 0.1, 1.0 }));
    }
    for (int x = -1; x <= 1; x++) {
      float y = t * 2 + 2;
      Box3 window = box3_extrude_from_point( { position.x, y, position.y+x*2.0f }, { 4.05, 0.75, 0.5 });
      boxdraw_push(&boxdraw, boxdraw_cmdgradient(window, { 0.0, 0.0, 1.0, 1.0 }, { 1.0, 0.0, 0.1, 1.0 }));
    }
  }
}

World global_world;
Placement current_placement;
bool show_buildings = true;

void frame(void) {
  const int width = sapp_width();
  const int height = sapp_height();

  camera.set_aspect((float)width / height);
  if (sapp_mouse_locked()) {
    camera_input_apply(&camera, &inputs);
  }

  if (inputs.key_states[SAPP_KEYCODE_X].pressed) {
    show_buildings = !show_buildings;
  }
  bheight += inputs.mouse_wheel;

  if (bheight < 1) bheight = 1;
  if (bheight > 9) bheight = 9;


  Matrix4 view_projection = camera.vp;
  
  simgui_new_frame( { width, height, sapp_frame_duration(), sapp_dpi_scale() });

  static Vector3 p = { 0.0, 0.0, 0.0 };


  // the imgui UI pass
  flashbacks_gui.show();

  ImGui::SetNextWindowPos( { 0, 600 });
  ImGui::Begin("debug");
  ImGui::Text("%g\n", 1 / sapp_frame_duration());
  ImGui::ColorPicker4("Colour", (float*)&boxdraw.pass_action.colors[0].value, ImGuiColorEditFlags_PickerHueWheel);
  
  ImGui::DragFloat("yaw", &camera.yaw);
  ImGui::DragFloat("pitch", &camera.pitch);
  ImGui::Text("oof %d", flashbacks.dialogs[0].taken);
  ImGui::End();

  ImDrawList *draw_list = ImGui::GetBackgroundDrawList();

  draw_list->AddCircle( { width / 2.0f, height / 2.0f }, 4, 0xFFFFFFFF);
  PlacementGrid grid = {};
 
  Vector2 positions[3] = { { 0, 0 }, { 9, 0 }, { 9, 9 } };
  int heights[3] = { 3, 4, 1 };

  for (int i = 0; i < global_world.buildings_count; i++) {
    Building building = global_world.buildings[i];
    place_building(&grid, building.x + 16, building.y + 16, true);
    if (show_buildings) {
      render_building(building.x, building.y, building.h);
    }
  }


  int rx, ry;
  if (map_ray_to_grid(camera.ray(), &rx, &ry)) {
    if (place_building(&grid, rx, ry, false)) {
      if (show_buildings) {
        render_building(rx - 16, ry - 16, bheight);
      }
      if (inputs.mouse_states[0].pressed) {
        world_place_building(&global_world, Building { rx - 16, ry - 16, bheight });
      }
    }
  }

  render_grid(&grid);


  Box3 floor = box3_extrude_from_point({ 0, 0, 0 }, { 50, 0.1, 50 });
  boxdraw_push(&boxdraw, boxdraw_cmdcolored(floor, { 0.6, 0.6, 0.3, 1.0 }));
 
  for (
    SceneIterator iterator = scene_iterator_begin(&game_scene);
    scene_iterator_going(&iterator);
    scene_iterator_next(&iterator))
  {
    Vector3 position = iterator.item.entity->position;

    Box3 object_box = box3_extrude_from_point(position, {1, 1, 1});
    Vector4 color_multiple = {1, 1, 1, 1};
    Ray3 camera_ray = camera.ray();
    

    if (ray3_vs_box3_slow(camera_ray, object_box, 5)) {
      if (inputs.mouse_states[0].released) {
        sapp_lock_mouse(false);
        flashbacks_gui.begin_sequence(iterator.item.entity->dialog_id);
      }
      color_multiple = {.2, .2, .2, 1};
    }

    Vector4 color_top = Vector4{ 0, 0, 1, 1 } * color_multiple;
    Vector4 color_bottom = Vector4{ 1, 0, 1, 1 } * color_multiple;

    boxdraw_push(&boxdraw, boxdraw_cmdgradient(object_box, color_top, color_bottom));
  }
  
  boxdraw_flush(&boxdraw, view_projection);
  {
    sg_pass_action pass_action = {};
    pass_action.colors[0].action = SG_ACTION_LOAD;
    pass_action.colors[0].value = { 0.3f, 0.7f, 0.5f, 1.0f };
  
    sg_begin_default_pass(&pass_action, width, height);
    simgui_render(); 
    sg_end_pass();
  }
  
  sg_commit();

  inputs.update();
}


void cleanup(void) {
  boxdraw_destroy(&boxdraw);
  simgui_shutdown();
  sg_shutdown();
}


void input(const sapp_event* event) {
  if (!simgui_handle_event(event)) {
    if (event->type == SAPP_EVENTTYPE_KEY_UP && event->key_code == SAPP_KEYCODE_ESCAPE) {
      sapp_lock_mouse(false);
    }
    if (!sapp_mouse_locked() && event->type == SAPP_EVENTTYPE_MOUSE_UP && event->mouse_button == 0) {
      sapp_lock_mouse(true);
    } else {
      inputs.pass_event(event);
    }
  }
}


sapp_desc sokol_main(int argc, char* argv[]) {
  (void)argc; (void)argv;
  sapp_desc desc = { };
  desc.init_cb = init;
  desc.frame_cb = frame;
  desc.cleanup_cb = cleanup;
  desc.event_cb = input;
  desc.width = 1024;
  desc.height = 768;
  desc.gl_force_gles2 = true;
  desc.window_title = "Education tool";
  desc.ios_keyboard_resizes_canvas = false;
  desc.icon.sokol_default = true;
  desc.enable_clipboard = true;
  desc.logger.func = slog_func;
  return  desc;
}
