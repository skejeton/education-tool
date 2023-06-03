#include "entry.hpp"
#include "entity_editor.hpp"
#include "scene.hpp"

static void set_mode(Entry *entry, PlayingMode mode) {
  entry->playing_mode = mode;

  switch (mode) {
    case PLAYING_MODE_PLAY:
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
    Rect building_rect = {{building_pos.x-4, building_pos.y-4},  {8, 8}};

    if (rect_vs_rect(camera_rect, building_rect)) {
      snap = rect_vs_rect_snap(camera_rect, building_rect);
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
}

static void save_entity_in_editor(Entry *entry) {
  Entity *prev_entity = scene_get_entity(&entry->scene, entry->entity_selected);
  if (prev_entity) {
    entry->entity_editor.emplace(&entry->flashbacks, &prev_entity->dialog_id);
  }
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
  if (inputs.key_states[SAPP_KEYCODE_1].pressed) {
    set_mode(this, PLAYING_MODE_BUILD);
  }
  if (inputs.key_states[SAPP_KEYCODE_2].pressed) {
    set_mode(this, PLAYING_MODE_PLAY);
  }

  if (inputs.key_states[SAPP_KEYCODE_X].pressed) {
    show_buildings = !show_buildings;
  }
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

  if (playing_mode == PLAYING_MODE_PLAY) {
    flashbacks_gui.show();
  } else {
    entity_editor.show();
  }

  ImGui::SetNextWindowPos( { 0, 400 });
  ImGui::Begin("debug");
  ImGui::Text("%g\n", 1 / sapp_frame_duration());
  ImGui::ColorPicker4("Colour", (float*)&boxdraw.pass_action.colors[0].clear_value, ImGuiColorEditFlags_PickerHueWheel);
  
  ImGui::DragFloat("yaw", &camera.yaw);
  ImGui::DragFloat("pitch", &camera.pitch);
  ImGui::LabelText("window_width", "%d", width);
  ImGui::LabelText("window_height", "%d", height);
  ImGui::DragInt("cmdc", &cmdc);
  ImGui::Text("pmode %d", playing_mode);
  ImGui::End();


  ImDrawList *draw_list = ImGui::GetBackgroundDrawList();

  draw_list->AddCircle( { width / 2.0f / sapp_dpi_scale(), height / 2.0f / sapp_dpi_scale() }, 4, 0xFFFFFFFF);

  
	// Draw Ground
  Box3 floor = box3_extrude_from_point({ 0, 0, 0 }, { 5000, 0.05, 5000 });
  boxdraw_push(&boxdraw, boxdraw_cmdcolored(floor, { 0.0, 0.8, 0.0, 1.0 }));

  auto pointing_at = PointingAtResolve::init(camera.ray());
  auto pointing_at_bindings = TempIdBinder<ObjectLocator>::init();

  for (int i = 0; i < world.buildings_count; i++) {
    Building building = world.buildings[i];
    int id = pointing_at_bindings.allocate({ ObjectLocator::Pool::BUILDING, i });
    pointing_at.push_box(building.box(), id);
  }

  if (playing_mode == PLAYING_MODE_BUILD) {
    PlacementGrid grid = PlacementGrid::init(camera.position.x, camera.position.z);
 
    int rx, ry;
    float rt = grid.map_ray(camera.ray(), &rx, &ry);
    Building potential_building = {rx, ry, bheight};

    bool may_place = rt < 50;
    if (last_object_locator.id != -1) {
      may_place = false;
    }

    for (int i = 0; i < world.buildings_count; i++) {
      Building building = world.buildings[i];
      grid.place_region(building.region());
    }

    bool can_place = may_place && grid.try_place_region(potential_building.region());

    grid.render(&boxdraw);

    if (can_place) {
      if (show_buildings) {
        potential_building.render(&boxdraw);
      }
      if (inputs.mouse_states[0].pressed && sapp_mouse_locked()) {
        world.add_building(potential_building);
      }
    }
  }

  if (show_buildings) {
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

    Box3 object_box = box3_extrude_from_point(position, {1, 1, 1});
    Vector4 color_multiple = {1, 1, 1, 1};
    
    if (last_object_locator.pool == ObjectLocator::Pool::ENTITY && (int)iterator.index == last_object_locator.id) {
      color_multiple = {0.5, 0.5, 0.5, 1.0};
    }

    if (entity_selected.index == iterator.item.id.index) {
      color_multiple = {0.3, 0.3, 0.3, 1.0};
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
          if (playing_mode == PLAYING_MODE_BUILD) {
            Building building = world.buildings[id];

            Box3 box = building.box();
            box.max.y = box.min.y + 0.3;
            box.min.x -= 0.3;
            box.min.z -= 0.3;
            box.max.x += 0.3;
            box.max.z += 0.3;

            boxdraw_push(&boxdraw, boxdraw_cmdcolored(box, {0, 0, 1, 1}));

            if (inputs.mouse_states[1].pressed) {
              world.remove_building(id);
            }  
          }
        } break;
        case ObjectLocator::Pool::ENTITY: {
          if (inputs.mouse_states[0].released) {
            Entity *entity = scene_get_entity(&scene, {(size_t)id+1});

            if (playing_mode == PLAYING_MODE_PLAY) {
              sapp_lock_mouse(false);
              flashbacks_gui.begin_sequence(entity->dialog_id);
            } else if (playing_mode == PLAYING_MODE_BUILD) {
              save_entity_in_editor(this);
              this->entity_selected = {(size_t)id+1};
              this->entity_editor = EntityEditor::derive_from(&flashbacks, entity->dialog_id);
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