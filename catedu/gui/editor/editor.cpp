#include "editor.hpp"
#include "catedu/genobj/building.hpp"
#include "catedu/genobj/grid.hpp"
#include "catedu/genobj/render.hpp"
#include "catedu/misc/camera_input.hpp"
#include "catedu/rendering/3d/pbr.hpp"
#include "catedu/rendering/render_model.hpp"
#include "catedu/sys/fs/open_dir.hpp"
#include "catedu/ui/widgets.hpp"
#include "offscreen.hpp"
#include <umka_api.h>

enum ObjectInteractionEvent
{
    None,
    Select,
    Delete,
};

bool icon_button(UiUser &user, const char *name, const char *icon,
                 Vector4 color = {1.0, 1.0, 1.0, 1.0}, float scale = 1);

void do_action(GuiEditor &editor, Scene &scene, EditAction action,
               bool discard = true)
{
    Object *obj;
    switch (action.type)
    {
    case EditAction::CreateEntity:
        editor.selection = scene.add_object(action.cmd.create_entity.entity);
        action.cmd.create_entity.entity_id = editor.selection;
        break;
    case EditAction::DeleteEntity:
        action.cmd.delete_entity.entity_data =
            scene.prune_object(action.cmd.delete_entity.entity_id);
        if (editor.selection == action.cmd.delete_entity.entity_id)
        {
            editor.selection = {};
        }
        break;
    case EditAction::MoveEntity:
        obj = scene.get_object(action.cmd.move_entity.entity);
        assert(obj);
        action.cmd.move_entity.current_pos = obj->entity.pos;
        obj->entity.pos = action.cmd.move_entity.pos;
        break;
    case EditAction::PlaceTile:
        obj = scene.get_object(action.cmd.place_tile.tilemap_entity);
        assert(obj);
        assert(obj->type == Object::Type::Tilemap);
        action.cmd.place_tile.prev_id =
            obj->tilemap.get_tile(action.cmd.place_tile.pos);
        obj->tilemap.set_tile(action.cmd.place_tile.pos,
                              action.cmd.place_tile.tile_id);
        break;
    default:
        assert(false);
        break;
    }

    editor.dirty = true;

    if (editor.action_buoy < (sizeof editor.actions / sizeof editor.actions[0]))
    {
        editor.actions[editor.action_buoy++] = action;
        if (discard)
        {
            editor.action_count = editor.action_buoy;
        }
    }
}

void redo_action(GuiEditor &editor, Scene &scene, bool firstcall = true)
{
    if (editor.action_buoy >= editor.action_count)
    {
        return;
    }

    EditAction action = editor.actions[editor.action_buoy];

    if (!action.final || firstcall)
    {
        do_action(editor, scene, action, false);
        redo_action(editor, scene, false);
    }
}

void undo_action(GuiEditor &editor, Scene &scene)
{
    if (editor.action_buoy <= 0)
    {
        return;
    }

    EditAction action = editor.actions[--editor.action_buoy];

    Object *obj;
    switch (action.type)
    {
    case EditAction::CreateEntity:
        scene.remove_object(action.cmd.create_entity.entity_id);
        if (editor.selection == action.cmd.create_entity.entity_id)
        {
            editor.selection = {};
        }
        break;
    case EditAction::DeleteEntity:
        scene.add_object(action.cmd.delete_entity.entity_data);
        break;
    case EditAction::MoveEntity:
        obj = scene.get_object(action.cmd.move_entity.entity);
        assert(obj);
        obj->entity.pos = action.cmd.move_entity.current_pos;
        break;
    case EditAction::PlaceTile:
        obj = scene.get_object(action.cmd.place_tile.tilemap_entity);
        assert(obj);
        assert(obj->type == Object::Type::Tilemap);
        obj->tilemap.set_tile(action.cmd.place_tile.pos,
                              action.cmd.place_tile.prev_id);
        break;
    default:
        assert(false);
        break;
    }

    if (!action.final)
    {
        undo_action(editor, scene);
    }

    editor.dirty = true;
}

AutoLayoutElement create_main_element(UiUser &user)
{
    AutoLayoutElement element = {};
    element.position = AutoLayoutPosition::Absolute;
    element.width.type = AutoLayoutDimension::Pixel;
    element.width.value = sapp_widthf() / user.state->dpi_scale;
    element.height.type = AutoLayoutDimension::Pixel;
    element.height.value = sapp_heightf() / user.state->dpi_scale;
    return element;
}

enum class RectSide
{
    Left,
    Right,
    Top,
    Bottom
};

bool rect_side_is_horizontal(RectSide &side)
{
    return side == RectSide::Bottom || side == RectSide::Top;
}

void begin_toolbar(UiUser &user, const char *name, RectSide side)
{
    float align_y = 0, align_x = 0;

    if (rect_side_is_horizontal(side))
    {
        align_y = side == RectSide::Bottom;
        align_x = 0.5;
    }
    else
    {
        align_x = side == RectSide::Right;
        align_y = 0.5;
    }

    AutoLayoutElement element = create_main_element(user);
    element.align_height = align_y;
    element.align_width = align_x;
    user.begin_generic(element, {}, {});
    user.push_id(name);

    AutoLayoutElement toolbar = {};
    toolbar.layout.type =
        rect_side_is_horizontal(side) ? AutoLayout::Row : AutoLayout::Column;
    toolbar.width.type = AutoLayoutDimension::Auto;
    toolbar.height.type = AutoLayoutDimension::Auto;
    toolbar.padding = {2, 2, 2, 2};
    toolbar.margin = {2, 2, 2, 2};
    toolbar.border = {1, 1, 1, 1};
    user.begin_generic(toolbar, UiMakeBrush::make_solid({1.0, 1.0, 1.0, 0.6}),
                       {}, user.state->element_storage.id());
}

void end_toolbar(UiUser &user)
{
    user.end_generic();
    user.pop_id();
    user.end_generic();
}

bool icon_button(UiUser &user, const char *name, const char *icon,
                 Vector4 color, float scale)
{
    AutoLayoutElement el = {};
    el.padding = {2, 2, 2, 2};
    el.margin = {2, 2, 2, 2};
    el.border = {1, 1, 1, 1};

    begin_button_frame(user, name, el, color);
    img(user, icon, {scale, scale});
    return end_button_frame(user);
}

void show_editor_mode(UiUser &user, EditorTab &active_tab)
{
    begin_toolbar(user, "Mode", RectSide::Top);

    user.collection(AutoLayout::Row, [&] {
        button_radio(user, "Config", (int &)active_tab, EDITOR_TAB_CONFIG);
        button_radio(user, "Build", (int &)active_tab, EDITOR_TAB_BUILD);
        button_radio(user, "Characters", (int &)active_tab,
                     EDITOR_TAB_CHARACTERS);
        button_radio(user, "Script", (int &)active_tab, EDITOR_TAB_SCRIPT);
    });

    end_toolbar(user);
}

void check_dirty(GuiEditor &editor, bool edited)
{
    editor.dirty = edited || editor.dirty;
}

void show_config_panel(UiUser &user, Scene &scene, GuiEditor &editor)
{
    label(user, "World Configuration", {1.5, 1.5},
          UiMakeBrush::make_solid({0.0f, 0.0f, 0.5f, 1.0f}));

    label(user, "Title");
    check_dirty(editor, input(user, "title", scene.name, sizeof(scene.name)));

    label(user, "Description");
    check_dirty(editor, input(user, "description", scene.description,
                              sizeof(scene.description)));

    label(user, "Backdrop");
    check_dirty(editor, button_radio(user, "Interior", (int &)scene.backdrop,
                                     BACKDROP_VOID));
    check_dirty(editor, button_radio(user, "Exterior", (int &)scene.backdrop,
                                     BACKDROP_GRASS));
}

void show_script_panel(UiUser &user, Scene &scene, GuiEditor &editor)
{
    label(user, "Script Configuration", {1.5, 1.5},
          UiMakeBrush::make_solid({0.0f, 0.0f, 0.5f, 1.0f}));

    if (button(user, "Open script directory"))
    {
#ifdef _WIN32
        catedu::sys::open("assets\\script");
#else
        catedu::sys::open("assets/script");
#endif
    }

    label(user, "Object References");
    for (auto [id, obj] : iter(scene.objects))
    {
        if (obj.type == Object::Type::Entity)
        {
            if (strcmp(obj.id, "") != 0)
            {
                if (button(user, obj.id))
                {
                    editor.selection = id;
                    editor.tab = EDITOR_TAB_BUILD;
                    editor.dirty = true;
                }
            }
        }
    }
}

GenResources get_genres(ResourceSpec &resources)
{
    GenResources result = {};
    result.box =
        resources.models.get(resources.find_model_by_name("cube"))->model;
    return result;
}

bool object_icon_button(UiUser &user, const char *name, Vector4 color,
                        catedu::pbr::Renderer &renderer,
                        ResourceSpec &resources)
{
    AutoLayoutElement el = {};
    el.width = {AutoLayoutDimension::Pixel, 190};
    el.height = {AutoLayoutDimension::Pixel, 150};
    el.align_width = 0.5;
    el.align_height = 0.5;
    el.padding = {2, 2, 2, 2};
    el.margin = {2, 2, 2, 2};
    el.border = {1, 1, 1, 1};

    UiImageId id;

    begin_button_frame(user, name, el, color);
    {
        Camera camera = Camera::init(5);
        camera.set_aspect(190.f / 150.f);

        camera.move(0, 0, -200);
        camera.rotate_around({0, 0, 0}, 45, -45);
        camera.move(0, 5, 0);

        renderer.camera = camera;
        renderer.begin_pass_offscreen(offscreen_pass_action(),
                                      offscreen_alloc(id));

        GeneratedObject obj = genmesh_generate_building(5);
        genobj_render_object(renderer, get_genres(resources), obj);

        renderer.end_pass();
    }

    img(user, id, {1, 1});
    return end_button_frame(user);
}

GuiEditor GuiEditor::init(UiState *ui_state)
{
    Camera camera = Camera::init(45);
    camera.move(0, 10, 0);
    camera.rotate(0, -55);

    GuiEditor result = {};
    result.ui_state = ui_state;
    result.camera = camera;
    result.debug_tree = GuiDebugTree::init();
    result.tab = EDITOR_TAB_CONFIG;
    result.world = World::create();

    printf("Init editor\n");

    offscreen_init_targets(ui_state->core);

    return result;
}

SelectionState show_selection(GuiEditor &editor,
                              catedu::pbr::Renderer &renderer,
                              ResourceSpec &resources, Scene &scene,
                              Input &input)
{
    float window_width = sapp_widthf(), window_height = sapp_heightf();

    float distance;
    Ray3 ray = editor.camera.screen_to_world_ray(
        input.mouse_pos, Vector2{window_width, window_height});

    ObjTilemap *tilemap_selected = nullptr;
    Vector2 pos = {};
    Vector3 pos3d = {};
    bool is_selected = editor.placing_object;
    TableId selector_model_id = resources.find_model_by_name("selector");
    TableId model_id = NULL_ID;
    int rotation = 0;

    if (is_selected)
    {
        pos3d.x = editor.object_cursor_at.x;
        pos3d.z = editor.object_cursor_at.y;
        pos = editor.object_cursor_at;
    }
    else if (ray3_vs_horizontal_plane(ray, -0.5, &distance))
    {
        pos3d = ray3_at(ray, distance);
        pos = {pos3d.x, pos3d.z};

        Object *selected = scene.get_object(editor.selection);
        if (selected)
        {
            if (selected->type == Object::Type::Tilemap)
            {
                pos.x = round(pos.x);
                pos.y = round(pos.y);

                tilemap_selected = &selected->tilemap;

                if (editor.tilemap_edit.tile != NULL_ID)
                {
                    SpecTile &tile =
                        resources.tiles.get_assert(editor.tilemap_edit.tile);
                    rotation = tile.rotation;
                    model_id = tile.model_id;
                }
            }
        }

        pos3d.x = pos.x;
        pos3d.z = pos.y;
        pos3d.y = 0;

        is_selected = true;
    }
    if (is_selected)
    {
        if (model_id != NULL_ID)
        {
            render_model_at(pos3d, resources, model_id, renderer, true, true,
                            rotation);
        }
        else
        {
            render_model_at(pos3d, resources, selector_model_id, renderer,
                            true);
        }
    }

    return {tilemap_selected, pos};
}

ObjectId find_object_within_distance(Scene &scene, Vector2 pos, float distance)
{
    for (auto [id, obj] : iter(scene.objects))
    {
        if (obj.type == Object::Type::Entity)
        {
            Vector2 obj_pos = obj.entity.pos;
            if (vector2_cmp_distance(pos, obj_pos) < distance)
            {
                return id;
            }
        }
    }
    return NULL_ID;
}

void GuiEditor::start_playtest(Scene &scene, bool *reload_module, void *umka)
{
    switch (Playtest::from_scene_copy(this->playtest, scene, camera))
    {
    case PlaytestCreateError::None:
        this->playtesting = true;
        *reload_module = true;
        break;
    case PlaytestCreateError::NoPlayer:
        this->playtest_no_player = true;
        break;
    default:
        assert(false);
        break;
    }
}

void GuiEditor::stop_playtest(Playtest &playtest)
{
    playtest.deinit();
    this->playtesting = false;
}

bool handle_camera_movement(Camera &camera, Input &input, UiUser &user)
{
    if (camera.position.y > 5 && input.mouse_wheel > 0)
    {
        camera.move(0, -input.mouse_wheel * 2, input.mouse_wheel * 2);
    }
    if (camera.position.y < 40 && input.mouse_wheel < 0)
    {
        camera.move(0, -input.mouse_wheel * 2, input.mouse_wheel * 2);
    }
    if (input.k[INPUT_MB_MIDDLE].held)
    {
        camera.move(-input.mouse_delta.x / (20 * user.state->dpi_scale), 0,
                    input.mouse_delta.y / (20 * user.state->dpi_scale));
        sapp_lock_mouse(true);
        return true;
    }
    else
    {
        sapp_lock_mouse(false);
        return false;
    }
}

void save_scene(Scene &scene, const char *path)
{
    Buffer data = scene.save();
    FILE *file = fopen(path, "wb");
    fwrite(data.data, 1, data.size, file);
    fclose(file);
    free(data.data);
}

void GuiEditor::save(Scene &scene)
{
    save_scene(scene, "assets/world.dat");
    dirty = false;
}

void handle_shortcuts(GuiEditor &editor, Scene &scene, Input &input)
{
    if (input.shortcut(MOD_CTRL, SAPP_KEYCODE_Z))
    {
        undo_action(editor, scene);
    }
    if (input.shortcut(MOD_CTRL, SAPP_KEYCODE_Y))
    {
        redo_action(editor, scene);
    }
    if (input.shortcut(MOD_CTRL, SAPP_KEYCODE_S))
    {
        editor.save(scene);
    }
}

void show_build_panel(UiUser &user, GuiEditor &editor, ResourceSpec &resources,
                      catedu::pbr::Renderer &renderer, Scene &scene)
{
    object_icon_button(user, "Building", {1.0, 1.0, 1.0, 1.0}, renderer,
                       resources);
}

void show_left_panel(UiUser &user, GuiEditor &editor, ResourceSpec &resources,
                     catedu::pbr::Renderer &renderer, Scene &scene)
{
    AutoLayoutElement element = {};
    element.clip = true;
    element.width = {AutoLayoutDimension::Pixel, 200};
    element.height = {AutoLayoutDimension::Pixel,
                      sapp_screen_rect_scaled(user.state->dpi_scale).siz.y};
    user.state->element_storage.push("Left Panel", {});
    user.begin_generic(element,
                       UiMakeBrush::make_gradient({1.0f, 1.0f, 1.0f, 0.6f},
                                                  {1.0f, 1.0f, 1.0f, 0.7f}),
                       {}, user.state->element_storage.id());
    switch (editor.tab)
    {
    case EDITOR_TAB_CONFIG:
        show_config_panel(user, scene, editor);
        break;
    case EDITOR_TAB_BUILD:
        show_build_panel(user, editor, resources, renderer, scene);
        break;
    case EDITOR_TAB_SCRIPT:
        show_script_panel(user, scene, editor);
        break;
    default:;
    }

    user.end_generic();
    user.state->element_storage.pop();
}

bool GuiEditor::show_build_mode(UiUser &user, catedu::pbr::Renderer &renderer,
                                ResourceSpec &resources, Scene &scene,
                                void *umka, bool *reload_module,
                                SelectionState &sel)
{
    Input &input = this->ui_state->input;

    if (input.k[SAPP_KEYCODE_F3].pressed ||
        input.k[SAPP_KEYCODE_SCROLL_LOCK].pressed)
    {
        this->show_debug = !this->show_debug;
    }

    debug_tree.reset();
    debug_tree.value("Camera Pos X", renderer.camera.position.x);
    debug_tree.value("Camera Pos Y", renderer.camera.position.y);
    debug_tree.value("Camera Pos Z", renderer.camera.position.z);
    debug_tree.value("Mouse Pos X", input.mouse_pos.x);
    debug_tree.value("Mouse Pos Y", input.mouse_pos.y);
    debug_tree.value("Dirty", dirty);

    return false;
}

void show_popups(UiUser &user, GuiEditor &editor, bool &return_back)
{
    if (editor.exit_requested)
    {
        const char *options[] = {"Yes", "No", NULL};
        switch (msgbox(user, "Exit requested",
                       "Are you sure you want to exit?\nAll unsaved changes "
                       "will be lost.",
                       MsgBoxType::Warning, options))
        {
        case 0:
            editor.exit_requested = false;
            editor.dirty = false;
            sapp_request_quit();
            break;
        case 1:
            editor.exit_requested = false;
            break;
        }
    }

    if (return_back)
        editor.tried_to_return_back = true;

    if (editor.tried_to_return_back && editor.dirty)
    {
        return_back = false;
        const char *options[] = {"Yes", "No", NULL};
        switch (msgbox(user, "Return to main menu",
                       "Are you sure you want to return to the main menu?\nAll "
                       "unsaved changes will be lost.",
                       MsgBoxType::Warning, options))
        {
        case 0:
            editor.dirty = false;
            return_back = true;
            break;
        case 1:
            editor.tried_to_return_back = false;
            break;
        }
    }

    if (editor.show_debug)
    {
        WindowInfo info = {"Debug", {0, 0, 300, 400}};
        begin_show_window(user, info);
        editor.debug_tree.show(user);
        end_show_window(user);
    }
}

void show_playtest_controls(UiUser &user, GuiEditor &editor)
{
    begin_toolbar(user, "Controls", RectSide::Bottom);

    if (icon_button(user, "Stop playtest", "assets/gui/stop.png"))
    {
        editor.stop_playtest(editor.playtest);
    }

    end_toolbar(user);
}

void show_editor_controls(UiUser &user, GuiEditor &editor, Scene &scene,
                          bool *reload_module, void *umka, bool &return_back)
{
    begin_toolbar(user, "Controls", RectSide::Bottom);

    if (icon_button(user, "Home", "assets/gui/home.png"))
    {
        return_back = true;
    }

    if (icon_button(user, "Undo", "assets/gui/undo.png"))
    {
        undo_action(editor, scene);
    }

    if (icon_button(user, "Start playtest", "assets/gui/play.png"))
    {
        editor.start_playtest(scene, reload_module, umka);
    }

    if (icon_button(user, "Redo", "assets/gui/redo.png"))
    {
        redo_action(editor, scene);
    }

    Vector4 color = editor.dirty ? Vector4{1.0, 0.4, 0.4, 1.0}
                                 : Vector4{1.0, 1.0, 1.0, 1.0};
    if (icon_button(user, "Save", "assets/gui/save.png", color))
    {
        editor.save(scene);
    }

    end_toolbar(user);
}

void show_playtest_ui(GuiEditor &editor, UiUser &user, ResourceSpec &resources,
                      catedu::pbr::Renderer &renderer, Input &input,
                      bool *reload_module, void *umka)
{
    renderer.camera = editor.playtest.camera;
    renderer.begin_pass();
    editor.playtest.handle_update(input, resources, umka);
    editor.playtest.handle_render(renderer, resources, editor.show_debug);
    editor.playtest.handle_gui(user, resources, reload_module, umka);
    renderer.end_pass();

    show_playtest_controls(user, editor);
}

SelectionState show_editor_ui(GuiEditor &editor, UiUser &user,
                              ResourceSpec &resources,
                              catedu::pbr::Renderer &renderer, Scene &scene,
                              Input &input, bool *reload_module, void *umka,
                              bool &return_back)
{
    handle_shortcuts(editor, scene, input);

    editor.camera.set_aspect(sapp_widthf() / sapp_heightf());
    renderer.camera = editor.camera;
    renderer.begin_pass();

    SelectionState sel = {};
    scene.render(renderer, resources);

    // Handle main scene interactions
    sel = show_selection(editor, renderer, resources, scene, input);
    editor.object_cursor_at = sel.position;

    if (editor.tab == EDITOR_TAB_BUILD)
    {
        GenResources gen_resources = get_genres(resources);
        if (user.hovered())
        {
            static int floors = 3;

            floors += input.k[SAPP_KEYCODE_UP].pressed;
            floors -= input.k[SAPP_KEYCODE_DOWN].pressed;

            floors = clamp(floors, 2, 10);
            if (input.k[INPUT_MB_LEFT].pressed)
            {
                editor.world.add_building(floors, roundf(sel.position.x),
                                          roundf(sel.position.y));
            }
            if (input.k[INPUT_MB_RIGHT].pressed)
            {
                editor.world.remove_building(roundf(sel.position.x),
                                             roundf(sel.position.y));
            }

            GeneratedObject building = genmesh_generate_building(floors);
            genobj_render_object(renderer, gen_resources, building,
                                 Matrix4::translate({roundf(sel.position.x), 0,
                                                     roundf(sel.position.y)}));

            for (int i = 0; i < editor.world.num_buildings; i++)
            {
                Building &building = editor.world.buildings[i];
                GeneratedObject buildingobj =
                    genmesh_generate_building(building.floors);
                genobj_render_object(renderer, gen_resources, buildingobj,
                                     Matrix4::translate({roundf(building.x), 0,
                                                         roundf(building.y)}));
            }
        }

        Ray3 ray = editor.camera.screen_to_world_ray({0.5, 0.5}, {1, 1});
        float t;
        ray3_vs_horizontal_plane(ray, 0, &t);
        Vector2 offs = {round(ray.origin.x + ray.direction.x * t),
                        round(ray.origin.z + ray.direction.z * t)};

        GeneratedObject grid = genmesh_generate_grid(round(t), round(t));
        genobj_render_object(renderer, gen_resources, grid,
                             Matrix4::translate({offs.x, 0, offs.y}));
    }

    if (user.hovered())
    {
        handle_camera_movement(editor.camera, input, user);
    }

    renderer.end_pass();

    show_editor_mode(user, editor.tab);
    show_editor_controls(user, editor, scene, reload_module, umka, return_back);
    show_left_panel(user, editor, resources, renderer, scene);

    if (editor.tab == EDITOR_TAB_BUILD)
    {
        editor.show_build_mode(user, renderer, resources, scene, umka,
                               reload_module, sel);
    }

    return sel;
}

bool show_main_editor(GuiEditor &editor, UiUser &user, ResourceSpec &resources,
                      catedu::pbr::Renderer &renderer, Scene &scene,
                      Input &input, bool *reload_module, void *umka)
{
    AutoLayoutElement element = create_main_element(user);
    user.state->element_storage.push("Main", {});
    user.begin_generic(element, {}, {}, user.state->element_storage.id());

    bool return_back = false;

    if (editor.playtesting)
    {
        show_playtest_ui(editor, user, resources, renderer, input,
                         reload_module, umka);
    }
    else
    {
        show_editor_ui(editor, user, resources, renderer, scene, input,
                       reload_module, umka, return_back);
    }

    user.end_generic();
    user.state->element_storage.pop();

    return return_back;
}

bool GuiEditor::show(catedu::pbr::Renderer &renderer, ResourceSpec &resources,
                     Scene &scene, UiUser &user, void *umka,
                     bool *reload_module)
{
    offscreen_clear();

    if (user.state->input.shortcut(MOD_ALT, SAPP_KEYCODE_1))
    {
        this->tab = EDITOR_TAB_CONFIG;
    }
    if (user.state->input.shortcut(MOD_ALT, SAPP_KEYCODE_2))
    {
        this->tab = EDITOR_TAB_BUILD;
    }
    if (user.state->input.shortcut(MOD_ALT, SAPP_KEYCODE_3))
    {
        this->tab = EDITOR_TAB_CHARACTERS;
    }
    if (user.state->input.shortcut(MOD_ALT, SAPP_KEYCODE_4))
    {
        this->tab = EDITOR_TAB_SCRIPT;
    }

    bool return_back = show_main_editor(*this, user, resources, renderer, scene,
                                        user.state->input, reload_module, umka);

    show_popups(user, *this, return_back);

    return return_back;
}

void GuiEditor::deinit()
{
    offscreen_deinit_targets(this->ui_state->core);

    debug_tree.deinit();
    if (this->playtesting)
    {
        this->playtest.deinit();
    }
}
