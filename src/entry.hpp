///
/// BRIEF: Entry point for everything
///

#ifndef H_CATEDU_ENTRY
#define H_CATEDU_ENTRY

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
#include "placement_grid.hpp"
#include "world.hpp"
#include "help_menu.hpp"
#include <memory>
#include <stdio.h>
#include "quest.hpp"

#include "entity_editor.hpp"

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

enum class SelectionOption {
  ENTITY,
  BUILDING,
  PAVEMENT,
  TREE,
  COUNT
};

struct Entry {
  Input inputs;
  Camera camera;
  Vector3 camera_velocity;
  Scene scene;
  Flashbacks flashbacks;
  FlashbacksGui flashbacks_gui;
  BoxdrawRenderer boxdraw;
  int bheight = 0;
  int cmdc = 0;
  World world;
  bool show_objects = true;
  PlayingMode playing_mode;
  ObjectLocator last_object_locator = {};
  EntityId entity_selected;
  EntityEditor entity_editor;
  SelectionOption selection_option;
  int stage;
  ImFont *main_font;
  HelpMenu help_menu;
  ObjectiveList objective_list;
  EntityId last_entity_interacted;

  void init(); 
  void input(const sapp_event* event);
  void cleanup();
  void frame();
};

#endif // H_CATEDU_ENTRY