#include "entry.hpp"
#include "boxdraw.hpp"
#include "entity_editor.hpp"
#include "imgui/imgui.h"
#include "math.hpp"
#include "placement_grid.hpp"
#include "scene.hpp"
#include "character.hpp"
#include "save.hpp"
#include <cstdlib>
#include <filesystem>
#include "project.hpp"
#include "easy_gui.hpp"

static void save_entity_in_editor(Entry *entry)
{
    entry->entity_editor.emplace();
}

static void set_mode(Entry *entry, PlayingMode mode)
{
    entry->playing_mode = mode;

    switch (mode) {
        case PLAYING_MODE_MENU:
            entry->open_project.is_open = false;
            entry->main_menu = MainMenu::init(std::filesystem::current_path() / "user");
            break;
        case PLAYING_MODE_PLAY:
            entry->flashbacks.backlog = {};
            entry->camera_velocity = {0, 0, 0};
            break;
        case PLAYING_MODE_BUILD:
            break;
    }
}

static void load_entry_project(Entry *entry, Project *project)
{
    entry->scene = std::move(project->scene);
    entry->flashbacks = std::move(project->flashbacks);
}

static void load_project_entry(Entry *entry, Project *project)
{
    project->scene = std::move(entry->scene);
    project->flashbacks = std::move(entry->flashbacks);
}

static void write_open_save(Entry *entry)
{
    Project project = {};
    load_project_entry(entry, &project);
    auto project_file = ProjectFile::init_from_path(entry->open_project.path.c_str());
    project.save(project_file);
    project_file.deinit();
}

static void load_open_save(Entry *entry)
{
    Project project = {};
    load_project_entry(entry, &project);
    auto project_file = ProjectFile::init_from_path(entry->open_project.path.c_str());
    project.save(project_file);
    project_file.deinit();
}

static void handle_game_mode_input(Entry *entry, Input *inputs)
{
    if (inputs->key_states[SAPP_KEYCODE_LEFT_BRACKET].pressed) {
        save_entity_in_editor(entry);
        set_mode(entry, PLAYING_MODE_BUILD);
    }
    if (inputs->key_states[SAPP_KEYCODE_RIGHT_BRACKET].pressed) {
        set_mode(entry, PLAYING_MODE_PLAY);
    }

    if (inputs->key_states[SAPP_KEYCODE_F2].pressed) {
        load_open_save(entry);
    }

    if (inputs->key_states[SAPP_KEYCODE_F1].pressed) {
        save_entity_in_editor(entry);
        write_open_save(entry);
    }

    if (inputs->key_states[SAPP_KEYCODE_X].pressed) {
        entry->show_objects = !entry->show_objects;
    }
}


static bool is_game_mode(PlayingMode playing_mode)
{
    return playing_mode == PLAYING_MODE_BUILD || playing_mode == PLAYING_MODE_PLAY;
}


static void handle_in_game_input(Entry *entry, Input *inputs)
{
    switch (entry->playing_mode) {
        case PLAYING_MODE_PLAY:
            if (inputs->key_states[SAPP_KEYCODE_SPACE].pressed && entry->camera.position.y < 2.1) {
                entry->camera_velocity.y = 10;
            }
            camera_input_apply(&entry->camera, inputs, false);
            break;
        case PLAYING_MODE_BUILD:
            camera_input_apply(&entry->camera, inputs);
            break;
        default:
            // Not an in-game mode.
            break;
    }

}


static void handle_input(Entry *entry, Input *inputs) {
    if (is_game_mode(entry->playing_mode)) {
        handle_game_mode_input(entry, inputs);
    }

    if (sapp_mouse_locked()) {
        handle_in_game_input(entry, inputs);
    }
}


void check_collisions(Entry *entry) {
    Rect camera_rect = {{entry->camera.position.x-1, entry->camera.position.z-1}, {2, 2}};
    Vector2 snap = {0, 0};

    for (SceneIterator iterator = scene_iterator_begin(&entry->scene);
         scene_iterator_going(&iterator);
         scene_iterator_next(&iterator))
    {
        Entity* entity = scene_get_entity(&entry->scene, iterator.id);
        Rect collision_rect = entity_collision_rect(entity);

        if (rect_vs_rect(camera_rect, collision_rect)) {
            snap = rect_vs_rect_snap(camera_rect, collision_rect);
            break;
        }
    }

    entry->camera.position.x += snap.x;
    entry->camera.position.z += snap.y;
}

static void set_imgui_rounding(float rounding) {
    ImGui::GetStyle().TabRounding = rounding;
    ImGui::GetStyle().ChildRounding = rounding;
    ImGui::GetStyle().FrameRounding = rounding;
    ImGui::GetStyle().GrabRounding = rounding-1;
    ImGui::GetStyle().WindowRounding = rounding;
    ImGui::GetStyle().ScrollbarRounding = rounding;
    ImGui::GetStyle().PopupRounding = rounding;
}

static void init_imgui_font(Entry *entry, const char *path, float size) {
    auto io = ImGui::GetIO();
    entry->main_font = io.Fonts->AddFontFromFileTTF(path, sapp_dpi_scale()*size);
    uint8_t *font_pixels;
    int font_width, font_height;

    io.Fonts->GetTexDataAsRGBA32(&font_pixels, &font_width, &font_height);
    sg_image_desc img_desc = {};
    img_desc.width = font_width;
    img_desc.height = font_height;
    img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    img_desc.wrap_u = SG_WRAP_CLAMP_TO_EDGE;
    img_desc.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
    img_desc.min_filter = SG_FILTER_LINEAR;
    img_desc.mag_filter = SG_FILTER_LINEAR;
    img_desc.data.subimage[0][0].ptr = font_pixels;
    img_desc.data.subimage[0][0].size = font_width * font_height * 4;
    io.Fonts->TexID = (ImTextureID)(uintptr_t) sg_make_image(&img_desc).id;
}

//--------------

void Entry::init(void) {
    console_create_or_bind_existing();

    set_mode(this, PLAYING_MODE_MENU);

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
    init_imgui_font(this, "./assets/Roboto-Regular.ttf", 16);
    set_imgui_rounding(5);
    ImGui::GetStyle().WindowPadding = {15, 15};
    ImGui::GetStyle().CellPadding = {3, 3};
    ImGui::GetStyle().FramePadding = {3, 3};

    boxdraw = boxdraw_create();

    camera = Camera::init(45);
    camera.move(10, 10, 10);

    this->entity_editor = EntityEditor::init(&flashbacks);
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

static Entity generate_entity_from_type(SelectionOption selection, Vector3 position, int height)
{
    Entity ent = {};
    ent.interaction_type = EntityInteractionType::STATIC;

    // TODO: Instead of using enums everywhere, have models and for selection options use data.
    switch (selection) {
        case SelectionOption::BUILDING:
            ent.shape.type = ShapeType::BUILDING;
            break;
        case SelectionOption::ENTITY:
            ent.shape.type = ShapeType::CHARACTER;
            ent.interaction_type = EntityInteractionType::CHARACTER;
            break;
        case SelectionOption::PAVEMENT:
            ent.shape.type = ShapeType::PAVEMENT;
            break;
        case SelectionOption::TREE:
            ent.shape.type = ShapeType::TREE;
            break;
        default:;
    }

    ent.position = position;
    ent.shape.height = height;
    ent.shape.color = {1.0, 1.0, 1.0, 1.0};
    return ent;
}

void put_information_window(InformationWindowData data) {
    const int width = sapp_width();

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

void render_entity(BoxdrawRenderer *renderer, Entity *entity, bool selected, bool hovered) {
    Vector4 color_multiple = {1, 1, 1, 1};

    if (selected) {
        color_multiple = {0.3, 0.3, 0.3, 1.0};
    }

    if (hovered) {
        render_selection_box_base(renderer, entity_get_box(entity));
    }

    entity_render(renderer, entity);
}

static void show_ui_game_mode(Entry *entry)
{
    const float width = sapp_widthf();
    const float height = sapp_heightf();


    EasyGui gui = {};

//    gui.margin = 10;
    gui.padding = 20;

    gui.begin({width, height});
    gui.begin_layout_cut(Layout::COLUMN, Side::RIGHT, 400);
    gui.stretch = true;
    entry->help_menu.show(&gui);
    gui.label("hi");
    gui.stretch = false;
    gui.end_layout();
    gui.begin_layout(Layout::ROW);

    if (gui.button("EXIT")) {
        set_mode(entry, PLAYING_MODE_MENU);
    }
    if (entry->playing_mode == PLAYING_MODE_BUILD) {
        if (gui.button("PLAY")) {
            save_entity_in_editor(entry);
            set_mode(entry, PLAYING_MODE_PLAY);
        }
    } else {
        if (gui.button("BUILD")) {
            set_mode(entry, PLAYING_MODE_BUILD);
        }
        if (gui.button("Backlog")) {
            entry->flashbacks_gui.toggle_backlog();
        }
    }
    gui.right_to_left = true;
    if (gui.button("HELP")) {
        entry->help_menu.shown = !entry->help_menu.shown;
    }

    gui.end_layout();
    gui.end();

    switch (entry->playing_mode) {
        case PLAYING_MODE_PLAY:
            break;
        case PLAYING_MODE_BUILD:
//            put_information_window({ entry->selection_option });
            if (entry->entity_selected.id != 0) {
                entry->entity_editor.show();
            }
            break;
        default:
            break;
    }

    ImDrawList *draw_list = ImGui::GetBackgroundDrawList();
    draw_list->AddCircle({ width / 2.0f, height / 2.0f }, 4, 0xFFFFFFFF);
}

static void draw_background(ImU32 color)
{
    ImDrawList *draw_list = ImGui::GetBackgroundDrawList();
    draw_list->AddRectFilled({0, 0}, {sapp_widthf(), sapp_heightf()}, color);
}

static void show_ui(Entry *entry)
{
    ImGui::PushFont(entry->main_font);

    if (is_game_mode(entry->playing_mode)) {
        show_ui_game_mode(entry);
    } else if (entry->playing_mode == PLAYING_MODE_MENU) {
        draw_background(0xFFFFFFFF);
        entry->main_menu.show(&entry->open_project);
        if (entry->open_project.is_open) {
            load_open_save(entry);
            set_mode(entry, PLAYING_MODE_BUILD);
        }
    }

    ImGui::PopFont();
}


static void handle_game_mode(Entry *entry)
{
    const int width = sapp_width();
    const int height = sapp_height();

    entry->camera.set_aspect((float)width / height);
    if (entry->camera.position.y < 0.2) {
        entry->camera.position.y = 0.2;
    }

    entry->selection_option = input_selection_option(entry->inputs, entry->selection_option);
    entry->bheight += entry->inputs.mouse_wheel;

    if (entry->bheight < 1) entry->bheight = 1;
    if (entry->bheight > 9) entry->bheight = 9;

    Matrix4 view_projection = entry->camera.vp;

    // Draw Ground
    Box3 floor = box3_extrude_from_point({ 0, 0, 0 }, { 5000, 0.05, 5000 });
    boxdraw_push(&entry->boxdraw, boxdraw_cmdcolored(floor, { 0.1, 0.8, 0.15, 1.0 }));

    auto pointing_at = PointingAtResolve::init(entry->camera.ray());
    auto pointing_at_bindings = TempIdBinder<TableId>::init();

    if (entry->playing_mode == PLAYING_MODE_BUILD) {
        PlacementGrid grid = PlacementGrid::init(entry->camera.position.x, entry->camera.position.z);

        for (
                SceneIterator iterator = scene_iterator_begin(&entry->scene);
                scene_iterator_going(&iterator);
                scene_iterator_next(&iterator))
        {
            grid.place_region(entity_placement_region(scene_get_entity(&entry->scene, iterator.id)));
        }

        int rx, ry;
        float rt = grid.map_ray(entry->camera.ray(), &rx, &ry);

        bool may_place = rt < 50;
        if (entry->last_object_locator.id > 0) {
            may_place = false;
        }

        Entity potential_entity = generate_entity_from_type(entry->selection_option, { (float)rx, 0, (float)ry }, entry->bheight);

        bool can_place = may_place && grid.try_place_region(entity_placement_region(&potential_entity));

        grid.render(&entry->boxdraw);

        if (can_place) {
            if (entry->show_objects) {
                render_entity(&entry->boxdraw, &potential_entity, false, false);
            }
            if (entry->inputs.mouse_states[0].pressed && sapp_mouse_locked()) {
                scene_summon_entity(&entry->scene, potential_entity);
            }
        }
    }

    for (
            SceneIterator iterator = scene_iterator_begin(&entry->scene);
            scene_iterator_going(&iterator);
            scene_iterator_next(&iterator))
    {
        Entity* entity = scene_get_entity(&entry->scene, iterator.id);
        bool selected = false, hovered = false;

        if (iterator.id.id == entry->last_object_locator.id) {
            hovered = true;
        }

        if (entry->entity_selected.id == iterator.id.id) {
            selected = true;
        }

        int id = pointing_at_bindings.allocate(iterator.id);
        pointing_at.push_box(entity_get_box(entity), id);

        if (entry->show_objects) {
            render_entity(&entry->boxdraw, entity, selected, hovered);
        }
    }

    TableId *locator = pointing_at_bindings.get(pointing_at.closest_id());
    entry->last_object_locator.id = 0;
    if (locator != nullptr) {
        entry->last_object_locator = *locator;
        int id = locator->id;
        if (id >= 0) {
            TableId entity_id = entry->last_object_locator;
            Entity *entity = scene_get_entity(&entry->scene, entity_id);
            if (entry->playing_mode == PLAYING_MODE_PLAY) {
                if (entry->inputs.mouse_states[0].released) {
                    sapp_lock_mouse(false);
                    entry->flashbacks_gui.begin_sequence(entity->dialog_id);
                }
            } else if (entry->playing_mode == PLAYING_MODE_BUILD) {
                if (entry->inputs.mouse_states[0].released) {
                    save_entity_in_editor(entry);
                    entry->entity_selected = entity_id;
                    entry->entity_editor.derive_from(entity);
                } else if (entry->inputs.mouse_states[1].pressed) {
                    if (entry->entity_selected.id == id) {
                        entry->entity_selected.id = 0;
                        entry->entity_editor = EntityEditor::init(&entry->flashbacks);
                    }

                    scene_remove_entity(&entry->scene, entity_id);
                }
            }
        }
    }

    entry->cmdc = entry->boxdraw.commands_count;
    boxdraw_flush(&entry->boxdraw, view_projection);
}

void update_mode(Entry *entry) {
    switch (entry->playing_mode) {
        case PLAYING_MODE_PLAY: {
            handle_game_mode(entry);
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
            handle_game_mode(entry);
            break;
        case PLAYING_MODE_MENU:
            break;
    }
}

void Entry::frame(void) {
    const int width = sapp_width();
    const int height = sapp_height();

    handle_input(this, &inputs);
    update_mode(this);
    simgui_new_frame({ width, height, sapp_frame_duration(), 1 });
    show_ui(this);
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
            if (is_game_mode(this->playing_mode)) {
                sapp_lock_mouse(true);
            }
        } else {
            inputs.pass_event(event);
        }
    }
}
