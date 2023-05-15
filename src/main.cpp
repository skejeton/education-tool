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
#include "pointing_at_resolve.hpp"
#include "temp_id_binder.hpp"
#include <memory>
#include <stdio.h>

//--------------

struct Building {
  int x, y, h;
};

struct World {
  Building buildings[256];
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

enum PlayingMode {
  PLAYING_MODE_BUILD,
  PLAYING_MODE_PLAY,
};

struct ObjectLocator {
  // Which pool the location belongs to
  enum class Pool {
    BUILDING, ENTITY
  } pool;
  int id;
};

//--------------

static Input inputs;
static Camera camera;
static Vector3 camera_velocity;
static Scene game_scene;
static Flashbacks flashbacks;
static FlashbacksGui flashbacks_gui;
static BoxdrawRenderer boxdraw;
static int bheight = 0;
static int cmdc = 0;
static World global_world;
static bool show_buildings = true;
static PlayingMode g_playing_mode;

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

static const Vector4 placement_colorscheme[4] = {
  { 0.0f, 0.0f, 0.0f, 0.0f },
  { 1.0f, 0.0f, 0.5f, 1.0f },
  { 1.0f, 0.0f, 0.0f, 1.0f },
  { 0.0f, 1.0f, 0.0f, 1.0f },
};

//--------------

float map_ray_to_grid(Ray3 ray, int *x, int *y) {
  float ground_t;
  if (ray3_vs_horizontal_plane(ray, 0, &ground_t)) {
    Vector3 at = ray3_at(ray, ground_t);
    *x = at.x+16;
    *y = at.z+16;
    return ground_t;
  }
  return 100000;
}

bool tile_available(PlacementGrid *grid, int x, int y) {
  return x >= 0 && x < 32 && y >= 0 && y < 32;
}

bool can_place_building(int chunkx, int chunky, PlacementGrid *grid, int x, int y) {
  x -= chunkx*32;
  y -= chunky*32;
  for (int i = -4; i < 4; i++) {
    for (int j = -4; j < 4; j++) {
      if (tile_available(grid, i+x, j+y) && grid->tiles[i+x][j+y] == PLACEMENT_GRID_TILE_OCCUPIED) {
        return false;
      }
    }
  }

  return true;
}

void place_building(int chunkx, int chunky, PlacementGrid *grid, int x, int y, bool placed, bool can_place) {
  x -= chunkx*32;
  y -= chunky*32;
  if (placed) {
    for (int i = -4; i < 4; i++) {
      for (int j = -4; j < 4; j++) {
        if (tile_available(grid, i+x, j+y)) {
          grid->tiles[i+x][j+y] = PLACEMENT_GRID_TILE_OCCUPIED;
        }
      }
    }
  } else {
    for (int i = -4; i < 4; i++) {
      for (int j = -4; j < 4; j++) {
        if (tile_available(grid, i+x, j+y)) {
          grid->tiles[i+x][j+y] = can_place ? PLACEMENT_GRID_TILE_CANPLACE : PLACEMENT_GRID_TILE_CANTPLACE;
        }
      }
    }
  }
}

void world_place_building(World *world, Building building) {
  assert(world->buildings_count < 256 && "Out of buildings");
  world->buildings[world->buildings_count++] = building;
}

void world_remove_building(World *world, size_t index) {
  assert(index < world->buildings_count);

  memmove(world->buildings+index, world->buildings+index+1, sizeof(Building) * (world->buildings_count-index-1));
  world->buildings_count -= 1;
}

void render_grid(int chunkx, int chunky, PlacementGrid *grid, float alpha) {
  Vector4 grid_color = {1.0, 1.0, 0.0, alpha};

  for (int i = -16; i <= 16; i++) {
    Box3 box = box3_extrude_from_point( { (float)i+chunkx*32, 0.0f, chunky*32.0f }, { 0.05f, 0.15f, 16.0f });
    boxdraw_push(&boxdraw, boxdraw_cmdcolored(box, grid_color));
  }
  for (int i = -16; i <= 16; i++) {
    Box3 box = box3_extrude_from_point( { chunkx*32.0f, 0.0f, (float)i+chunky*32 }, { 16.0f, 0.15f, 0.05f });
    boxdraw_push(&boxdraw, boxdraw_cmdcolored(box, grid_color));
  }
  for (int i = 0; i < 32; i++) {
    for (int j = 0; j < 32; j++) {
      if (grid->tiles[i][j]) {
        int x = (i - 16)+chunkx*32;
        int y = (j - 16)+chunky*32;
        Box3 box = box3_extrude_from_point( { (float)x+0.5f, 0.0f, (float)y+0.5f }, { 0.5f, 0.13f, 0.5f });
        boxdraw_push(&boxdraw, boxdraw_cmdcolored(box, placement_colorscheme[grid->tiles[i][j]]));
      }
    }
  }
}

Box3 building_box(int x, int y, int h) {
  float building_height = h * 2 + 2;
  return box3_extrude_from_point({ (float)x, building_height/2, (float)y }, { 4, building_height/2, 4 });;
}

void render_building(int x, int y, int h) {
  Vector2 position = { (float)x, (float)y };
  float building_height = h * 2 + 2;
  Box3 building = building_box(x, y, h);
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

void set_mode(PlayingMode mode) {
  g_playing_mode = mode;

  switch (mode) {
    case PLAYING_MODE_PLAY:
      camera_velocity = {0, 0, 0};
      break;
    case PLAYING_MODE_BUILD:
      break;
  }
}

void handle_input(Input inputs) {
  switch (g_playing_mode) {
    case PLAYING_MODE_PLAY:
      if (inputs.key_states[SAPP_KEYCODE_SPACE].pressed && camera.position.y < 2.1) {
        camera_velocity.y = 10;
      }
      camera_input_apply(&camera, &inputs, false);
      break;
    case PLAYING_MODE_BUILD:
      camera_input_apply(&camera, &inputs);
      break;
  }
}

void check_collisions() {
  Rect camera_rect = {{camera.position.x-1, camera.position.z-1}, {2, 2}};
  Vector2 snap = {0, 0};

  for (int i = 0; i < global_world.buildings_count; i++) {
    Vector2 building_pos = {(float)global_world.buildings[i].x, (float)global_world.buildings[i].y};
    Rect building_rect = {{building_pos.x-4, building_pos.y-4},  {8, 8}};

    if (rect_vs_rect(camera_rect, building_rect)) {
      snap = rect_vs_rect_snap(camera_rect, building_rect);
      break;
    }
  }

  camera.position.x += snap.x;
  camera.position.z += snap.y;
}

void update_mode() {
  switch (g_playing_mode) {
    case PLAYING_MODE_PLAY: {
      camera_velocity.y -= 20*sapp_frame_duration();
      if (camera_velocity.y < -40) {
        camera_velocity.y = -40;
      }
      camera.position += camera_velocity*sapp_frame_duration();

      if (camera.position.y < 2) {
        camera.position.y = 2;
      }
      check_collisions();
    } break;
    case PLAYING_MODE_BUILD:
      break;
  }
}

//--------------

void init(void) {
  console_create_or_bind_existing();
  
#if 1
  EntityId ent1 = scene_summon_entity(&game_scene, { { 0, 1, 1 } });
  EntityId ent2 = scene_summon_entity(&game_scene, { { -6, 1, 1 } });

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

ObjectLocator last_object_locator = {};


void frame(void) {
  const int width = sapp_width();
  const int height = sapp_height();

  camera.set_aspect((float)width / height);
  if (sapp_mouse_locked()) {
    handle_input(inputs);
  }
  if (camera.position.y < 0.2) {
    camera.position.y = 0.2;
  }
  update_mode();
  if (inputs.key_states[SAPP_KEYCODE_1].pressed) {
    set_mode(PLAYING_MODE_BUILD);
  }
  if (inputs.key_states[SAPP_KEYCODE_2].pressed) {
    set_mode(PLAYING_MODE_PLAY);
  }

  if (inputs.key_states[SAPP_KEYCODE_X].pressed) {
    show_buildings = !show_buildings;
  }
  bheight += inputs.mouse_wheel;

  if (bheight < 1) bheight = 1;
  if (bheight > 9) bheight = 9;


  Matrix4 view_projection = camera.vp;
  
  simgui_new_frame( { width, height, sapp_frame_duration(), sapp_dpi_scale() });

  int rx, ry;
  float rt = map_ray_to_grid(camera.ray(), &rx, &ry);

  ImGui::GetIO().FontGlobalScale = 1.5;

  // the imgui UI pass
  if (ImGui::BeginMainMenuBar()) {
    if (g_playing_mode == PLAYING_MODE_BUILD) {
      if (ImGui::Button("PLAY")) {
        set_mode(PLAYING_MODE_PLAY);
      }
    } else {
      if (ImGui::Button("BUILD")) {
        set_mode(PLAYING_MODE_BUILD);
      }
      if (ImGui::Button("Backlog")) {
        flashbacks_gui.toggle_backlog();
      }
    }
    ImGui::EndMainMenuBar();
  }

  flashbacks_gui.show();

  ImGui::SetNextWindowPos( { 0, 400 });
  ImGui::Begin("debug");
  ImGui::Text("%g\n", 1 / sapp_frame_duration());
  ImGui::ColorPicker4("Colour", (float*)&boxdraw.pass_action.colors[0].value, ImGuiColorEditFlags_PickerHueWheel);
  
  ImGui::DragFloat("yaw", &camera.yaw);
  ImGui::DragFloat("pitch", &camera.pitch);
  ImGui::DragFloat("rt", &rt);
  ImGui::DragInt("cmdc", &cmdc);
  ImGui::Text("pmode %d", g_playing_mode);
  ImGui::End();


  ImDrawList *draw_list = ImGui::GetBackgroundDrawList();

  draw_list->AddCircle( { width / 2.0f, height / 2.0f }, 4, 0xFFFFFFFF);

  
  Box3 floor = box3_extrude_from_point({ 0, 0, 0 }, { 5000, 0.05, 5000 });
  boxdraw_push(&boxdraw, boxdraw_cmdcolored(floor, { 0.6, 0.6, 0.3, 1.0 }));

  bool can_place = rt < 50;
  bool may_place = rt < 50;
  if (last_object_locator.id != -1) {
    may_place = false;
  }

  auto pointing_at = PointingAtResolve::init(camera.ray());
  auto pointing_at_bindings = TempIdBinder<ObjectLocator>::init();

  for (int i = 0; i < global_world.buildings_count; i++) {
    Building building = global_world.buildings[i];
    Box3 box = building_box(building.x, building.y, building.h);

    int id = pointing_at_bindings.allocate({ ObjectLocator::Pool::BUILDING, i });
    pointing_at.push_box(box, id);
  }

  if (g_playing_mode == PLAYING_MODE_BUILD) {
    PlacementGrid grids[5][5] = {};

    for (int chunkx = -2; chunkx <= 2; chunkx++) {
      for (int chunky = -2; chunky <= 2; chunky++) {
        PlacementGrid *grid = &grids[chunkx + 2][chunky + 2];
        int ofsx = camera.position.x / 32;
        int ofsy = camera.position.z / 32;

        for (int i = 0; i < global_world.buildings_count; i++) {
          Building building = global_world.buildings[i];
          place_building(chunkx+ofsx, chunky+ofsy, grid, building.x + 16, building.y + 16, true, true);
        }

        can_place = can_place_building(chunkx+ofsx, chunky+ofsy, grid, rx, ry) && can_place && may_place;
      }
    }
    
    for (int chunkx = -2; chunkx <= 2; chunkx++) {
      for (int chunky = -2; chunky <= 2; chunky++) {
        PlacementGrid *grid = &grids[chunkx + 2][chunky + 2];
        int ofsx = camera.position.x / 32;
        int ofsy = camera.position.z / 32;
        float alpha = 1.0f-sqrt(chunkx*chunkx + chunky*chunky) / 2.0f + 0.2;

        if (may_place) {
          place_building(chunkx + ofsx, chunky + ofsy, grid, rx, ry, false, can_place);
        }

        render_grid(chunkx+ofsx, chunky+ofsy, grid, alpha);
      }
    }

    if (can_place) {
      if (show_buildings) {
        render_building(rx - 16, ry - 16, bheight);
      }
      if (inputs.mouse_states[0].pressed && sapp_mouse_locked()) {
        world_place_building(&global_world, Building { rx - 16, ry - 16, bheight });
      }
    }
  }

  if (show_buildings) {
    for (int i = 0; i < global_world.buildings_count; i++) {
      Building building = global_world.buildings[i];
      render_building(building.x, building.y, building.h);
    }
  }

  for (
    SceneIterator iterator = scene_iterator_begin(&game_scene);
    scene_iterator_going(&iterator);
    scene_iterator_next(&iterator))
  {
    Vector3 position = iterator.item.entity->position;

    Box3 object_box = box3_extrude_from_point(position, {1, 1, 1});
    Vector4 color_multiple = {1, 1, 1, 1};
    
    if (last_object_locator.pool == ObjectLocator::Pool::ENTITY && (int)iterator.index == last_object_locator.id) {
      color_multiple = {0.5, 0.5, 0.5, 1.0};
    }

    int id = pointing_at_bindings.allocate({ ObjectLocator::Pool::ENTITY, (int)iterator.index });
    pointing_at.push_box(object_box, id);

    Vector4 color_top = Vector4{ 0, 0, 1, 1 } * color_multiple;
    Vector4 color_bottom = Vector4{ 1, 0, 1, 1 } * color_multiple;

    boxdraw_push(&boxdraw, boxdraw_cmdgradient(object_box, color_top, color_bottom));
  }
  

  // Resolve "pointing at building"
  ObjectLocator *locator = pointing_at_bindings.get(pointing_at.closest_id());
  last_object_locator.id = -1;
  if (locator != nullptr) {
    last_object_locator = *locator;
    int id = locator->id;
    if (id >= 0) {
      switch (locator->pool) {
        case ObjectLocator::Pool::BUILDING: {
          if (g_playing_mode == PLAYING_MODE_BUILD) {
            Building building = global_world.buildings[id];

            Box3 box = building_box(building.x, building.y, building.h);
            box.max.y = box.min.y + 0.3;
            box.min.x -= 0.3;
            box.min.z -= 0.3;
            box.max.x += 0.3;
            box.max.z += 0.3;

            boxdraw_push(&boxdraw, boxdraw_cmdcolored(box, {0, 0, 1, 1}));

            if (inputs.mouse_states[1].pressed) {
              world_remove_building(&global_world, id);
            }  
          }
        } break;
        case ObjectLocator::Pool::ENTITY: {
          if (g_playing_mode == PLAYING_MODE_PLAY) {
            Entity *entity = scene_get_entity(&game_scene, {(size_t)id+1});
            if (inputs.mouse_states[0].released) {
              sapp_lock_mouse(false);
              flashbacks_gui.begin_sequence(entity->dialog_id);
            }
          }
        } break;
      }
    }
  }

 

  cmdc = boxdraw.commands_count;
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
