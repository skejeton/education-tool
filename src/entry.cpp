#include "entry.hpp"
#include "boxdraw.hpp"
#include "entity_editor.hpp"
#include "math.hpp"
#include "placement_grid.hpp"
#include "scene.hpp"
#include "world.hpp"

static void set_mode(Entry *entry, PlayingMode mode) {
  entry->playing_mode = mode;

  switch (mode) {
    case PLAYING_MODE_PLAY:
      entry->flashbacks.backlog = {};
      entry->camera_velocity = {0, 0, 0};
      break;
    case PLAYING_MODE_BUILD:
      break;
  }
}

static void handle_input(Entry *entry, Input inputs) {
  switch (entry->playing_mode) {
    case PLAYING_MODE_PLAY:
      if (inputs.key_states[SAPP_KEYCODE_SPACE].pressed && entry->camera.position.y < 2.1) {
        entry->camera_velocity.y = 10;
      }
      camera_input_apply(&entry->camera, &inputs, false);
      break;
    case PLAYING_MODE_BUILD:
      camera_input_apply(&entry->camera, &inputs);
      break;
  }
}

void check_collisions(Entry *entry) {
  Rect camera_rect = {{entry->camera.position.x-1, entry->camera.position.z-1}, {2, 2}};
  Vector2 snap = {0, 0};

  for (int i = 0; i < entry->world.buildings_count; i++) {
    Vector2 building_pos = {(float)entry->world.buildings[i].x, (float)entry->world.buildings[i].y};
    Rect collision_rect = entry->world.buildings[i].collision_rect();

    if (rect_vs_rect(camera_rect, collision_rect)) {
      snap = rect_vs_rect_snap(camera_rect, collision_rect);
      break;
    }
  }

  entry->camera.position.x += snap.x;
  entry->camera.position.z += snap.y;
}

void update_mode(Entry *entry) {
  switch (entry->playing_mode) {
    case PLAYING_MODE_PLAY: {
      entry->camera_velocity.y -= 20*sapp_frame_duration();
      if (entry->camera_velocity.y < -40) {
        entry->camera_velocity.y = -40;
      }
      entry->camera.position += entry->camera_velocity*sapp_frame_duration();

      if (entry->camera.position.y < 2) {
        entry->camera.position.y = 2;
      }
      check_collisions(entry);
    } break;
    case PLAYING_MODE_BUILD:
      break;
  }
}

//--------------

void Entry::init(void) {
  console_create_or_bind_existing();
  
#if 1
  scene_summon_entity(&scene, { { 0, 1, 1 } });
  scene_summon_entity(&scene, { { 0, 1, 9 } });
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

  this->entity_editor = EntityEditor::init(&flashbacks);
}

static void save_entity_in_editor(Entry *entry) {
  entry->entity_editor.emplace();
}

struct InformationWindowData {
  SelectionOption item_selection;
};

const char* selection_option_name(SelectionOption selection) {
  switch (selection) {
  case SelectionOption::ENTITY:
    return "Entity";
  case SelectionOption::BUILDING:
    return "Building";
  case SelectionOption::PAVEMENT:
    return "Pavement";
  case SelectionOption::TREE:
    return "Tree";
  default:
    return "Invalid";
  }
}

void put_information_window(InformationWindowData data) {
  const int width = sapp_width();
  const int height = sapp_height();

  ImGui::SetNextWindowPos({ (float)width - 300, 21 * sapp_dpi_scale() });
  ImGui::SetNextWindowSize({ 295, 300 });
  if (ImGui::Begin("Information")) {

    const char* selection = selection_option_name(data.item_selection);
    ImGui::LabelText("##sel", "Selected: %s", selection);
  
  }
  ImGui::End();
}

int input_inventory_index(Input input) {
  if (input.key_states[SAPP_KEYCODE_1].pressed) return 0;
  if (input.key_states[SAPP_KEYCODE_2].pressed) return 1;
  if (input.key_states[SAPP_KEYCODE_3].pressed) return 2;
  if (input.key_states[SAPP_KEYCODE_4].pressed) return 3;
  if (input.key_states[SAPP_KEYCODE_5].pressed) return 4;
  if (input.key_states[SAPP_KEYCODE_6].pressed) return 5;
  if (input.key_states[SAPP_KEYCODE_7].pressed) return 6;
  if (input.key_states[SAPP_KEYCODE_8].pressed) return 7;
  if (input.key_states[SAPP_KEYCODE_9].pressed) return 8;
  return -1;
}

SelectionOption input_selection_option(Input input, SelectionOption current) {
  int inventory_index = input_inventory_index(input);
  if (inventory_index >= 0 && inventory_index < (int)SelectionOption::COUNT) {
    return (SelectionOption)inventory_index;
  }
  return current;
}

void render_selection_box_base(BoxdrawRenderer *renderer, Box3 box) {
  box.max.y = box.min.y + 0.3;
  box.min.x -= 0.3;
  box.min.z -= 0.3;
  box.max.x += 0.3;
  box.max.z += 0.3;

  boxdraw_push(renderer, boxdraw_cmdcolored(box, {0, 0, 1, 1}));
}

Box3 entity_box(Entity *entity) {
  return box3_extrude_from_point(entity->position+Vector3{0, 0.5, 0}, {1, 1.5, 1});
}

void render_entity(BoxdrawRenderer *renderer, Entity *entity, bool selected, bool hovered) {
  Box3 box = entity_box(entity);
  Vector4 color_multiple = {1, 1, 1, 1};

  if (selected) {
    color_multiple = {0.3, 0.3, 0.3, 1.0};
  }

  if (hovered) {
    render_selection_box_base(renderer, box);
  }

  Vector4 color_top = Vector4{ 0, 0, 1, 1 } * color_multiple;
  Vector4 color_bottom = Vector4{ 1, 0, 1, 1 } * color_multiple;

  boxdraw_push(renderer, boxdraw_cmdgradient(box, color_top, color_bottom));
}

void Entry::frame(void) {
  const int width = sapp_width();
  const int height = sapp_height();

  camera.set_aspect((float)width / height);
  if (sapp_mouse_locked()) {
    handle_input(this, inputs);
  }
  if (camera.position.y < 0.2) {
    camera.position.y = 0.2;
  }
  update_mode(this);
  if (inputs.key_states[SAPP_KEYCODE_EQUAL].pressed) {
    stage++;
  }
  if (inputs.key_states[SAPP_KEYCODE_MINUS].pressed) {
    stage--;
  }
  if (stage < 0) stage = 0;
  if (stage > 8) stage = 8;
  if (inputs.key_states[SAPP_KEYCODE_LEFT_BRACKET].pressed) {
    set_mode(this, PLAYING_MODE_BUILD);
  }
  if (inputs.key_states[SAPP_KEYCODE_RIGHT_BRACKET].pressed) {
    set_mode(this, PLAYING_MODE_PLAY);
  }

  if (inputs.key_states[SAPP_KEYCODE_X].pressed) {
    show_objects = !show_objects;
  }
  selection_option = input_selection_option(inputs, selection_option);
  bheight += inputs.mouse_wheel;

  if (bheight < 1) bheight = 1;
  if (bheight > 9) bheight = 9;


  Matrix4 view_projection = camera.vp;
  
  simgui_new_frame({ width, height, sapp_frame_duration(), 1 });

  ImGui::GetIO().FontGlobalScale = sapp_dpi_scale();

  // the imgui UI pass
  if (ImGui::BeginMainMenuBar()) {
    if (playing_mode == PLAYING_MODE_BUILD) {
      if (ImGui::Button("PLAY")) {
        save_entity_in_editor(this);
        set_mode(this, PLAYING_MODE_PLAY);
      }
    } else {
      if (ImGui::Button("BUILD")) {
        set_mode(this, PLAYING_MODE_BUILD);
      }
      if (ImGui::Button("Backlog")) {
        flashbacks_gui.toggle_backlog();
      }
    }
    ImGui::EndMainMenuBar();
  }

  switch (playing_mode) {
  case PLAYING_MODE_PLAY:
    flashbacks_gui.show();
    break;
  case PLAYING_MODE_BUILD:
    put_information_window({ selection_option });
    if (entity_selected.index != 0) {
      entity_editor.show();
    }
    break;
  }

  ImGui::SetNextWindowPos( { 0, 400 });
  ImGui::Begin("debug");
  ImGui::Text("%g\n", 1 / sapp_frame_duration());
  ImGui::ColorPicker4("Colour", (float*)&boxdraw.pass_action.colors[0].clear_value, ImGuiColorEditFlags_PickerHueWheel);
  
  ImGui::DragFloat("yaw", &camera.yaw);
  ImGui::DragFloat("pitch", &camera.pitch);
  ImGui::LabelText("window_width", "%d", width);
  ImGui::LabelText("window_height", "%d", height);
  ImGui::LabelText("stage", "%d", stage+1);

  ImGui::DragInt("cmdc", &cmdc);
  ImGui::Text("pmode %d", playing_mode);
  ImGui::End();

  ImDrawList *draw_list = ImGui::GetBackgroundDrawList();

  draw_list->AddCircle( { width / 2.0f, height / 2.0f }, 4, 0xFFFFFFFF);

	// Draw Ground
  Box3 floor = box3_extrude_from_point({ 0, 0, 0 }, { 5000, 0.05, 5000 });
  boxdraw_push(&boxdraw, boxdraw_cmdcolored(floor, { 0.1, 0.8, 0.15, 1.0 }));

  auto pointing_at = PointingAtResolve::init(camera.ray());
  auto pointing_at_bindings = TempIdBinder<ObjectLocator>::init();

  for (int i = 0; i < world.buildings_count; i++) {
    Building building = world.buildings[i];
    int id = pointing_at_bindings.allocate({ ObjectLocator::Pool::BUILDING, i });
    pointing_at.push_box(building.box(), id);
  }

  if (playing_mode == PLAYING_MODE_BUILD) {
    PlacementGrid grid = PlacementGrid::init(camera.position.x, camera.position.z);
 
    for (int i = 0; i < world.buildings_count; i++) {
      Building building = world.buildings[i];
      grid.place_region(building.region());
    }

    for (int i = 0; i < SCENE_ENTITY_BUFFER_SIZE; i++) {
      if (scene.entities_taken[i]) {
        grid.place_region(entity_placement_region(&scene.entities[i]));
      }
    }

    PlacementRegion region;

    int rx, ry;
    float rt = grid.map_ray(camera.ray(), &rx, &ry);

    bool may_place = rt < 50;
    if (last_object_locator.id != -1) {
      may_place = false;
    }

    switch (this->selection_option) {
    case SelectionOption::ENTITY:
      {
        Entity potential_entity = {};
        potential_entity.position = { (float)rx, 1, (float)ry };

        bool can_place = may_place && grid.try_place_region(entity_placement_region(&potential_entity));

        grid.render(&boxdraw);

        if (can_place) {
          if (show_objects) {
            render_entity(&boxdraw, &potential_entity, false, false);
          }
          if (inputs.mouse_states[0].pressed && sapp_mouse_locked()) {
            scene_summon_entity(&scene, potential_entity);
          }
        }  
      } break;
    case SelectionOption::BUILDING: 
    case SelectionOption::PAVEMENT: 
    case SelectionOption::TREE: 
      {
        BuildingType building_type = BuildingType::APARTMENT;
        if (selection_option == SelectionOption::PAVEMENT) building_type = BuildingType::PAVEMENT;
        if (selection_option == SelectionOption::TREE) building_type = BuildingType::TREE;

        Building potential_building = {rx, ry, bheight, building_type};

        bool can_place = may_place && grid.try_place_region(potential_building.region());

        grid.render(&boxdraw);

        if (can_place) {
          if (show_objects) {
            potential_building.render(&boxdraw);
          }
          if (inputs.mouse_states[0].pressed && sapp_mouse_locked()) {
            world.add_building(potential_building);
          }
        }  
      } break;
    default:;
    }

  }

  if (show_objects) {
    for (int i = 0; i < world.buildings_count; i++) {
      Building building = world.buildings[i];
      building.render(&boxdraw);
    }
  }

  for (
    SceneIterator iterator = scene_iterator_begin(&scene);
    scene_iterator_going(&iterator);
    scene_iterator_next(&iterator))
  {
    Vector3 position = iterator.item.entity->position;
    bool selected = false, hovered = false;

    Vector4 color_multiple = {1, 1, 1, 1};
    
    if (last_object_locator.pool == ObjectLocator::Pool::ENTITY && (int)iterator.index == last_object_locator.id) {
      hovered = true;
    }

    if (entity_selected.index == iterator.item.id.index) {
      selected = true;
    }

    int id = pointing_at_bindings.allocate({ ObjectLocator::Pool::ENTITY, (int)iterator.index });
    pointing_at.push_box(entity_box(iterator.item.entity), id);

    if (show_objects) {
      render_entity(&boxdraw, iterator.item.entity, selected, hovered);
    }
  }

  // Resolve "pointing at building" 
  ObjectLocator *locator = pointing_at_bindings.get(pointing_at.closest_id());
  last_object_locator.id = -1;
  if (locator != nullptr) {
    last_object_locator = *locator;
    int id = locator->id;
    if (id >= 0) {
      switch (locator->pool) {
      case ObjectLocator::Pool::BUILDING:
        {
          if (playing_mode == PLAYING_MODE_BUILD) {
            Building building = world.buildings[id];

            render_selection_box_base(&boxdraw, building.box());

            if (inputs.mouse_states[1].pressed) {
              world.remove_building(id);
            }  
          }
        } break;
      case ObjectLocator::Pool::ENTITY:
        {
          Entity *entity = scene_get_entity(&scene, {(size_t)id+1});
          if (playing_mode == PLAYING_MODE_PLAY) {
            if (inputs.mouse_states[0].released) {
              sapp_lock_mouse(false);
              flashbacks_gui.begin_sequence(entity->dialog_stages_id[stage]);
            }
          } else if (playing_mode == PLAYING_MODE_BUILD) {
            if (inputs.mouse_states[0].released) {
              save_entity_in_editor(this);
              this->entity_selected = {(size_t)id+1};
              this->entity_editor.derive_from(entity);
            } else if (inputs.mouse_states[1].pressed) {
              if (this->entity_selected.index == id+1) {
                entity_selected.index = 0;
                this->entity_editor = EntityEditor::init(&flashbacks);
              }

              scene_remove_entity(&scene, {(size_t)id+1});
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
    pass_action.colors[0].load_action = SG_LOADACTION_LOAD;
    pass_action.colors[0].store_action = SG_STOREACTION_STORE;
    pass_action.colors[0].clear_value = { 0.3f, 0.7f, 0.5f, 1.0f };

    sg_begin_default_pass(&pass_action, width, height);
    simgui_render(); 
    sg_end_pass();
  }
  
  sg_commit();

  inputs.update();
}


void Entry::cleanup(void) {
  boxdraw_destroy(&boxdraw);
  simgui_shutdown();
  sg_shutdown();
}


void Entry::input(const sapp_event* event) {
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