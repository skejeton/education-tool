#include "editor.hpp"
#include "catedu/misc/camera_input.hpp"
#include "catedu/rendering/render_model.hpp"
#include "catedu/ui/widgets.hpp"
#include <umka_api.h>

void show_generic_icon(UiUser &user, const char *s, Vector4 color,
                       float w = 0.0f)
{
    AutoLayoutElement element = {};
    element.width.value = 16;
    element.width.type =
        w == 0.0f ? AutoLayoutDimension::Auto : AutoLayoutDimension::Pixel;
    element.height.value = 16;
    element.height.type = AutoLayoutDimension::Pixel;
    element.border = {1, 1, 1, 1};
    element.padding = {2, 2, 2, 2};
    element.margin = {2, 2, 2, 2};

    user.begin_generic(element, UiMakeBrush::make_solid(color),
                       UiMakeBrush::make_solid({0.0f, 0.0f, 0.0f, 1.0f}));

    label(user, s, {1, 1}, UiMakeBrush::make_solid({1.0f, 1.0f, 1.0f, 1.0f}));

    user.end_generic();
}

void show_generic_icon_char(UiUser &user, char c, Vector4 color)
{
    char s[3] = {' ', c, 0};
    show_generic_icon(user, s, color, 20.0f);
}

void show_object_icon(UiUser &user, Object::Type type)
{
    switch (type)
    {
    case Object::Type::Entity:
        show_generic_icon_char(user, 'E', {0.0f, 0.0f, 0.5f, 1.0f});
        break;
    case Object::Type::Tilemap:
        show_generic_icon_char(user, 'T', {0.0f, 0.5f, 0.0f, 1.0f});
        break;
    case Object::Type::Backdrop:
        show_generic_icon_char(user, 'B', {0.5f, 0.0f, 0.0f, 1.0f});
        break;
    case Object::Type::Generic:
        show_generic_icon_char(user, 'G', {0.2f, 0.2f, 0.2f, 1.0f});
        break;
    }
}

void show_object_icon_ex(UiUser &user, Object::Type type)
{
    switch (type)
    {
    case Object::Type::Entity:
        show_generic_icon(user, "Entity", {0.0f, 0.0f, 0.5f, 1.0f});
        break;
    case Object::Type::Tilemap:
        show_generic_icon(user, "Tilemap", {0.0f, 0.5f, 0.0f, 1.0f});
        break;
    case Object::Type::Backdrop:
        show_generic_icon(user, "Backdrop", {0.5f, 0.0f, 0.0f, 1.0f});
        break;
    case Object::Type::Generic:
        show_generic_icon(user, "Generic", {0.2f, 0.2f, 0.2f, 1.0f});
        break;
    }
}

enum ObjectInteractionEvent
{
    None,
    Select,
    Delete,
};

ObjectInteractionEvent show_object_row(UiUser &user, Object &obj,
                                       bool is_selected)
{
    ObjectInteractionEvent result = None;

    AutoLayoutElement element = {};
    element.layout.type = AutoLayout::Row;
    element.width.type = AutoLayoutDimension::Pixel;
    element.width.value = 200 - 8;
    element.height.type = AutoLayoutDimension::Auto;
    element.padding = {2, 2, 2, 2};
    element.margin = {1, 1, 1, 1};
    element.border = {0, 0, 1, 0};

    user.state->element_storage.push(obj.id, {});

    Vector4 bgc = {0.9f, 0.9f, 0.9f, 1.0f};
    if (is_selected)
    {
        bgc = {0.8f, 0.8f, 0.0f, 1.0f};
    }
    if (user.hovered())
    {
        bgc = {0.7f, 0.7f, 0.6f, 1.0f};
        if (user.state->input.k[INPUT_MB_LEFT].pressed)
        {
            result = Select;
        }
    }
    if (obj.hide)
    {
        bgc *= 0.5;
        bgc.w = 1.0f;
    }

    user.begin_generic(element, UiMakeBrush::make_solid(bgc),
                       UiMakeBrush::make_solid({0.0f, 0.0f, 0.0f, 1.0f}),
                       user.state->element_storage.id());

    if (button(user, "H"))
    {
        obj.hide = !obj.hide;
    }
    if (button(user, "X"))
    {
        result = Delete;
    }
    show_object_icon(user, obj.type);
    label(user, obj.name, {1, 1.2},
          UiMakeBrush::make_solid({0.0f, 0.0f, 0.0f, 1.0f}));

    user.end_generic();

    user.state->element_storage.pop();

    return result;
}

ObjectInteractionEvent show_object_properties(UiUser &user, Object &obj)
{
    return show_object_row(user, obj, false);
}

size_t show_pagination(UiUser &user, size_t page, size_t page_count)
{
    AutoLayoutElement element = {};
    element.layout.type = AutoLayout::Row;
    element.width.type = AutoLayoutDimension::Pixel;
    element.width.value = 200 - 8;
    element.height.type = AutoLayoutDimension::Auto;
    element.padding = {2, 2, 2, 2};
    element.margin = {1, 1, 1, 1};
    element.border = {0, 0, 1, 0};

    user.begin_generic(element,
                       UiMakeBrush::make_solid({0.9f, 0.9f, 0.9f, 1.0f}),
                       UiMakeBrush::make_solid({0.0f, 0.0f, 0.0f, 1.0f}));

    if (button(user, " First "))
    {
        page = 0;
    }
    if (button(user, " < ") && page != 0)
    {
        page -= 1;
    }
    if (button(user, " > "))
    {
        page += 1;
    }
    if (button(user, " Last "))
    {
        page = page_count;
    }

    if (page > page_count)
    {
        page = page_count;
    }

    user.end_generic();

    return page;
}

void select_object(GuiEditor &editor, TableId id)
{
    if (editor.selection != id)
    {
        editor.selection = id;
        editor.entity_list_page = id.id / 10;
    }
    else
    {
        editor.selection = {};
    }
}

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
        if (obj->tilemap.get_tile(action.cmd.place_tile.pos) ==
            action.cmd.place_tile.tile_id)
        {
            return;
        }
        obj->tilemap.set_tile(action.cmd.place_tile.pos,
                              action.cmd.place_tile.tile_id);
        break;
    default:
        assert(false);
        break;
    }

    editor.dirty = true;

    if (editor.action_buoy < 512)
    {
        editor.actions[editor.action_buoy++] = action;
        if (discard)
        {
            editor.action_count = editor.action_buoy;
        }
    }
}

void redo_action(GuiEditor &editor, Scene &scene)
{
    if (editor.action_buoy >= editor.action_count)
    {
        return;
    }

    EditAction action = editor.actions[editor.action_buoy];

    do_action(editor, scene, action, false);
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

    editor.dirty = true;
}

void show_help(UiUser &user, ResourceSpec &resources)
{
    user.bold = true;
    label(user, "On object screen: ", {1.2, 1.2},
          UiMakeBrush::make_solid({0.0f, 0.0f, 0.5f, 1.0f}));
    user.bold = false;
    label(user, "`>` - Select object");
    label(user, "`X` - Delete object");
    label(user, "`H` - Hide object");
    user.bold = true;
    label(user, "There's different types of objects:", {1.2, 1.2},
          UiMakeBrush::make_solid({0.0f, 0.0f, 0.5f, 1.0f}));
    user.bold = false;
    label(user, "[T] - Tilemap");
    label(user, "[E] - Entity");
    label(user, "[B] - Backdrop");
    user.bold = true;
    label(user, "Controls:", {1.2, 1.2},
          UiMakeBrush::make_solid({0.0f, 0.0f, 0.5f, 1.0f}));
    user.bold = false;
    label(user, "WASD - Move camera");
    label(user, "Middle click + drag - Move camera");
    label(user, "Scroll - Zoom in/out");
    label(user, "Tab - Debug window");
    label(user, "Back - Go back");
    label(user, "Hold LMB on window to move it");
    label(user, "Press RMB on window to collapse it");
    label(user, "Press LMB on the scene to place objects");
    label(user, "Press Ctrl +/- to zoom in/out the UI");
    user.bold = true;
    label(user, "Models:", {1.2, 1.2},
          UiMakeBrush::make_solid({0.0f, 0.0f, 0.5f, 1.0f}));
    user.bold = false;

    char model_names[512] = {};
    char *cur = model_names;
    int model_i = 0;
    for (auto [id, model] : iter(resources.models))
    {
        cur = strcat(cur, model.name);
        cur = strcat(cur, ", ");
        model_i++;
        if (model_i % 4 == 0)
        {
            cur = strcat(cur, "\n");
        }
    }

    label(user, model_names);
}

void show_place_object(UiUser &user, Scene &scene, GuiEditor &editor)
{
    Rect scr = sapp_screen_rect_scaled(editor.ui_state->dpi_scale);
    Rect rect = rect_center_rect(scr, {0, 0, 100, 200});
    Object new_obj = {};

    begin_show_window(user, {"Place object", rect});
    label(user, "Type:");
    if (button(user, "Entity"))
    {
        new_obj.type = Object::Type::Entity;
        strcpy(new_obj.name,
               stdstrfmt("Unnamed Entity %zu", scene.objects.count).c_str());
        new_obj.entity = ObjEntity::init("", editor.object_cursor_at);
    }
    if (button(user, "Tilemap"))
    {
        new_obj.type = Object::Type::Tilemap;
        new_obj.tilemap = ObjTilemap::init();
        strcpy(new_obj.name,
               stdstrfmt("Unnamed Tilemap %zu", scene.objects.count).c_str());
    }
    if (button(user, "Backdrop"))
    {
        new_obj.type = Object::Type::Backdrop;
        new_obj.backdrop = ObjBackdrop::init({0, 32, 32, 32});
        strcpy(new_obj.name,
               stdstrfmt("Unnamed Backdrop %zu", scene.objects.count).c_str());
    }
    if (button(user, "Cancel"))
    {
        editor.placing_object = false;
    }

    if (new_obj.type != Object::Type::Generic)
    {
        EditAction action = {};
        action.type = EditAction::CreateEntity;
        action.cmd.create_entity.entity = new_obj;

        do_action(editor, scene, action);
        editor.placing_object = false;
    }

    end_show_window(user);
}

void show_object_list(UiUser &user, GuiEditor &editor, Scene &scene)
{
    const size_t page_count =
        scene.objects.count > 0 ? (scene.objects.count - 1) / 10 : 0;
    editor.entity_list_page =
        show_pagination(user, editor.entity_list_page, page_count);

    size_t i = 0;

    for (auto [id, obj] : iter(scene.objects))
    {
        if (i++ < editor.entity_list_page * 10)
        {
            continue;
        }
        if (i > (editor.entity_list_page + 1) * 10)
        {
            break;
        }
        char idstr[256];
        sprintf(idstr, "%zu", id.id);

        user.state->element_storage.push(idstr, {});
        switch (show_object_row(user, obj, editor.selection == id))
        {
        case Select:
            select_object(editor, id);
            editor.dirty = true;
            break;
        case Delete: {
            EditAction action = {};
            action.type = EditAction::DeleteEntity;
            action.cmd.delete_entity.entity_id = id;
            do_action(editor, scene, action);
        }
        break;
        default:
            break;
        }
        user.state->element_storage.pop();
    }
}

void show_properties(UiUser &user, Object &obj, GuiEditor &editor)
{
    begin_show_window(user, {"Properties", {220, 20, 200, 200}});

    show_object_icon_ex(user, obj.type);
    label(user, "Name:", {1, 1},
          UiMakeBrush::make_solid({0.0f, 0.0f, 0.0f, 1.0f}));
    input(user, "SelObjName", obj.name, 32);
    label(user, "Id:", {1, 1},
          UiMakeBrush::make_solid({0.0f, 0.0f, 0.0f, 1.0f}));
    input(user, "SelObjId", obj.id, 32);
    if (obj.type == Object::Type::Entity)
    {
        label(user, "Model:", {1, 1},
              UiMakeBrush::make_solid({0.0f, 0.0f, 0.0f, 1.0f}));
        input(user, "SelObjModel", obj.entity.model_name, 32);
    }
    if (button(user, "OK"))
    {
        editor.selection = NULL_ID;
    }

    end_show_window(user);
}

void show_tile_picker(UiUser &user, ResourceSpec &resources, GuiEditor &editor)
{
    begin_show_window(
        user,
        {"Tiles", {sapp_widthf() / user.state->dpi_scale - 100, 20, 100, 800}});

    for (auto [id, tile] : iter(resources.tiles))
    {
        bool selected = editor.tile_selection == id;
        Vector4 background = selected ? Vector4{0.8, 1.0, 0.8, 1.0}
                                      : Vector4{1.0, 1.0, 1.0, 1.0};

        if (button(user, tile.name, background))
        {
            editor.tile_selection = selected ? NULL_ID : id;
        }
    }

    end_show_window(user);
}

GuiEditor GuiEditor::init(UiState *ui_state)
{
    Camera camera = Camera::init(45);
    camera.move(0, 10, 0);
    camera.rotate(0, -45);

    GuiEditor result = {};
    result.ui_state = ui_state;
    result.camera = camera;
    result.debug_tree = GuiDebugTree::init();

    return result;
}

struct SelectionState
{
    ObjTilemap *tilemap_selected;
    Vector2 position;
};

SelectionState show_selection(GuiEditor &editor, BoxdrawRenderer &renderer,
                              ResourceSpec &resources, Scene &scene,
                              Input &input)
{
    float window_width = sapp_widthf(), window_height = sapp_heightf();
    float aspect = window_width / window_height;

    float distance;
    Ray3 ray = editor.camera.screen_to_world_ray(
        input.mouse_pos, Vector2{window_width, window_height});

    ObjTilemap *tilemap_selected = nullptr;
    Vector2 pos = {};
    Vector3 pos3d = {};
    bool is_selected = editor.placing_object;
    TableId selector_model_id = resources.find_model_by_name("selector");
    TableId model_id = NULL_ID;

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

                if (editor.tile_selection != NULL_ID)
                {
                    SpecTile &tile =
                        resources.tiles.get_assert(editor.tile_selection);
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
            render_model_at(pos3d, resources, model_id, renderer, true, true);
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

ObjectId find_object_within_distance_not_player(Scene &scene, Vector2 pos,
                                                float distance)
{
    for (auto [id, obj] : iter(scene.objects))
    {
        if (obj.type == Object::Type::Entity)
        {
            Vector2 obj_pos = obj.entity.pos;
            if (vector2_cmp_distance(pos, obj_pos) < distance &&
                strcmp(obj.id, "player") != 0)
            {
                return id;
            }
        }
    }
    return NULL_ID;
}

bool GuiEditor::show(BoxdrawRenderer &renderer, ResourceSpec &resources,
                     Scene &scene, UiUser **user_out, void *umka,
                     bool *reload_module)
{
    UiUser user = UiUser::init(*this->ui_state);
    *user_out = &user;
    user.begin_pass();

    Input &input = this->ui_state->input;

    AutoLayoutElement element = {};
    element.position = AutoLayoutPosition::Absolute;
    element.width.type = AutoLayoutDimension::Pixel;
    element.width.value = sapp_widthf() / this->ui_state->dpi_scale;
    element.height.type = AutoLayoutDimension::Pixel;
    element.height.value = sapp_heightf() / this->ui_state->dpi_scale;
    user.state->element_storage.push("Main", {});
    user.begin_generic(element, {}, {}, user.state->element_storage.id());

    bool return_back = false;
    bool no_ui = false;

    debug_tree.reset();

    debug_tree.value("Mouse Pos X", input.mouse_pos.x);
    debug_tree.value("Mouse Pos Y", input.mouse_pos.y);
    debug_tree.value("Dirty", dirty);

    if (!this->playtesting)
    {
        if (camera.position.y > 5 && input.mouse_wheel > 0)
        {
            camera.move(0, -input.mouse_wheel * 2, input.mouse_wheel * 2);
        }
        if (camera.position.y < 40 && input.mouse_wheel < 0)
        {
            camera.move(0, -input.mouse_wheel * 2, input.mouse_wheel * 2);
        }
        if (input.k[INPUT_MB_MIDDLE].held && user.hovered())
        {
            camera.move(-input.mouse_delta.x / (20 * ui_state->dpi_scale), 0,
                        input.mouse_delta.y / (20 * ui_state->dpi_scale));
            sapp_lock_mouse(true);
            no_ui = true;
        }
        else
        {
            sapp_lock_mouse(false);
        }
    }

    if (this->playtesting)
    {
        this->playtest_scene->render(renderer, resources, true);
    }
    else
    {
        scene.render(renderer, resources);
    }

    camera.set_aspect(sapp_widthf() / sapp_heightf());
    if (!this->playtesting)
    {

        if (input.shortcut(INPUT_CTRL, SAPP_KEYCODE_Z))
        {
            undo_action(*this, scene);
        }
        if (input.shortcut(INPUT_CTRL, SAPP_KEYCODE_Y))
        {
            redo_action(*this, scene);
        }

        if (input.shortcut(INPUT_CTRL, SAPP_KEYCODE_S))
        {
            dirty = false;

            Buffer data = scene.save();
            FILE *file = fopen("assets/world.dat", "wb");
            fwrite(data.data, 1, data.size, file);
            fclose(file);
            free(data.data);
            // TODO: Save the scene
        }

        if (user.hovered())
        {
            camera_input_top_view_apply(&this->camera, &input);
        }

        if (user.hovered() || this->placing_object)
        {
            SelectionState sel =
                show_selection(*this, renderer, resources, scene, input);

            this->object_cursor_at = sel.position;

            if (sel.tilemap_selected && user.hovered() &&
                input.k[INPUT_MB_RIGHT].held)
            {
                EditAction action = {};
                action.type = EditAction::PlaceTile;
                action.cmd.place_tile.tilemap_entity = this->selection;
                action.cmd.place_tile.pos = vector2_to_vector2i(sel.position);
                action.cmd.place_tile.tile_id = 0;
                do_action(*this, scene, action);
            }

            if (sel.tilemap_selected && user.hovered() &&
                input.k[INPUT_MB_LEFT].held)
            {
                EditAction action = {};
                action.type = EditAction::PlaceTile;
                action.cmd.place_tile.tilemap_entity = this->selection;
                action.cmd.place_tile.pos = vector2_to_vector2i(sel.position);
                action.cmd.place_tile.tile_id = this->tile_selection.id;
                do_action(*this, scene, action);
            }

            if (input.k[INPUT_MB_LEFT].pressed && user.hovered())
            {
                ObjectId closest_object =
                    find_object_within_distance(scene, object_cursor_at, 1);

                if (closest_object != NULL_ID)
                {
                    this->selection = closest_object;
                }
                else if (!sel.tilemap_selected)
                {
                    if (this->selection != NULL_ID)
                    {
                        // Path: catedu/gui/editor/editor.cpp
                        Object *selected = scene.get_object(this->selection);
                        if (selected && selected->type == Object::Type::Entity)
                        {
                            EditAction action = {};
                            action.type = EditAction::MoveEntity;
                            action.cmd.move_entity.entity = this->selection;
                            action.cmd.move_entity.pos = this->object_cursor_at;
                            do_action(*this, scene, action);
                        }
                        else
                        {
                            this->placing_object = true;
                        }
                    }
                    else
                    {
                        this->placing_object = true;
                    }
                }
            }
        }
    }

    boxdraw_flush(&renderer, this->camera.vp);

    user.end_generic();
    this->ui_state->element_storage.pop();

    if (no_ui)
    {
        user.end_pass();
        return return_back;
    }

    if (button(user, "Back"))
    {
        return_back = true;
    }

    if (this->playtesting)
    {
        UmkaStackSlot empty;
        int func = umkaGetFunc(umka, NULL, "onUI");
        UmkaError error;
        if (umkaGetError(umka, &error), error.fnName[0] != 0)
        {
            if (!this->suppress_errors)
            {
                const char *buttons[] = {"Retry", "Ignore", NULL};

                switch (msgbox(user, "Umka Error",
                               stdstrfmt("%s\n    at %s:%d", error.msg,
                                         error.fileName, error.line)
                                   .c_str(),
                               MsgBoxType::Error, buttons))
                {
                case 0:
                    *reload_module = true;
                    break;
                case 1:
                    this->suppress_errors = true;
                    break;
                }
            }
        }
        else
        {
            umkaCall(umka, func, 0, NULL, &empty);
        }
    }

    if (button(user, this->playtesting ? "Exit playtest" : "Playtest"))
    {
        if (this->playtesting)
        {
            if (this->playtest_scene)
            {
                this->playtest_scene->deinit();
                free(this->playtest_scene);
            }
            this->playtesting = false;
        }
        else
        {
            if (scene.find_object("player") == NULL_ID)
            {
                this->playtest_no_player = true;
            }
            else
            {
                if (scene.get_object(scene.find_object("player"))->type !=
                    Object::Type::Entity)
                {
                    this->playtest_no_player = true;
                }
                else
                {
                    Scene original = scene.copy();
                    Scene *scene = (Scene *)malloc(sizeof(Scene));
                    memcpy(scene, &original, sizeof(Scene));
                    scene->update(resources);
                    this->playtest_scene = scene;
                    this->playtesting = true;
                    *reload_module = true;
                }
            }
        }
    }

    if (!this->playtesting)
    {
        if (button(user, "Undo"))
        {
            undo_action(*this, scene);
        }
        if (button(user, "Redo"))
        {
            redo_action(*this, scene);
        }

        char title[256];
        sprintf(title, "Objects | Page %zu", this->entity_list_page);

        begin_show_window(user, {title, {20, 20, 200, 410}});
        show_object_list(user, *this, scene);
        end_show_window(user);

        Object *selected = scene.get_object(this->selection);

        if (begin_show_window(user, {"Help", {0, 0, 250, 430}}))
        {
            show_help(user, resources);
        }
        end_show_window(user);

        if (selected)
        {
            show_properties(user, *selected, *this);
            if (selected->type == Object::Type::Tilemap)
            {
                show_tile_picker(user, resources, *this);
            }
        }

        if (this->placing_object)
        {
            show_place_object(user, scene, *this);
        }
    }
    else
    {
        Object *obj = this->playtest_scene->get_object(
            this->playtest_scene->find_object("player"));

        assert(obj);
        assert(obj->type == Object::Type::Entity);

        camera.position =
            Vector3{obj->entity.pos.x, 10, obj->entity.pos.y - 10};

        PhysicsBody *body =
            this->playtest_scene->physics.bodies.get(obj->entity.body_id);

        if (input.k[SAPP_KEYCODE_A].held)
        {
            body->area.pos.x -= 0.1;
        }
        if (input.k[SAPP_KEYCODE_D].held)
        {
            body->area.pos.x += 0.1;
        }
        if (input.k[SAPP_KEYCODE_W].held)
        {
            body->area.pos.y += 0.1;
        }
        if (input.k[SAPP_KEYCODE_S].held)
        {
            body->area.pos.y -= 0.1;
        }
        if (input.k[SAPP_KEYCODE_SPACE].pressed)
        {
            int func = umkaGetFunc(umka, NULL, "onInteract");

            const char *source = "";

            PhysicsManifolds manifolds =
                this->playtest_scene->physics.detect_collisions();

            ObjectId coll_id = NULL_ID;

            for (auto [id, manifold] : iter(manifolds.manifolds))
            {
                if (manifold.first == obj->entity.body_id)
                {
                    coll_id = manifold.second;
                    for (auto [id, obj] : iter(this->playtest_scene->objects))
                    {
                        if (obj.type == Object::Type::Entity)
                        {
                            if (obj.entity.body_id == coll_id)
                            {
                                source = obj.id;
                            }
                        }
                    }
                }
                else if (manifold.second == obj->entity.body_id)
                {
                    coll_id = manifold.first;
                    for (auto [id, obj] : iter(this->playtest_scene->objects))
                    {
                        if (obj.type == Object::Type::Entity)
                        {
                            if (obj.entity.body_id == coll_id)
                            {
                                source = obj.id;
                            }
                        }
                    }
                }
            }

            assert(func);
            UmkaStackSlot id;

            id.ptrVal = umkaMakeStr(umka, (char *)source);
            UmkaError error;
            if (umkaGetError(umka, &error), error.fnName[0] == 0)
            {
                umkaCall(umka, func, 1, &id, NULL);
            }
        }

        this->playtest_scene->update(resources);
    }

    if (input.k[SAPP_KEYCODE_TAB].held)
    {
        begin_show_window(user, {"Debug", {0, 0, 300, 400}});
        debug_tree.show(user);
        end_show_window(user);
    }

    if (this->playtest_no_player)
    {
        const char *options[] = {"Ok", NULL};
        switch (msgbox(user, "Error!",
                       "To playtest, add an entity with id 'player'.",
                       MsgBoxType::Error, options))
        {
        case 0:
            this->playtest_no_player = false;
        }
    }

    if (this->exit_requested)
    {
        const char *options[] = {"Yes", "No", NULL};
        switch (msgbox(user, "Exit requested",
                       "Are you sure you want to exit?\nAll unsaved changes "
                       "will be lost.",
                       MsgBoxType::Warning, options))
        {
        case 0:
            this->exit_requested = false;
            this->dirty = false;
            sapp_request_quit();
        case 1:
            this->exit_requested = false;
            break;
        }
    }

    if (return_back && this->dirty || this->tried_to_return_back)
    {
        this->tried_to_return_back = true;
        return_back = false;
        const char *options[] = {"Yes", "No", NULL};
        switch (msgbox(user, "Unsaved changes",
                       "Are you sure you want to go back?\nAll unsaved changes "
                       "will be lost.",
                       MsgBoxType::Warning, options))
        {
        case 0:
            this->dirty = false;
            return_back = true;
            this->tried_to_return_back = false;
            break;
        case 1:
            this->tried_to_return_back = false;
            break;
        }
    }

    if (*reload_module)
    {
        this->suppress_errors = false;
    }

    *user_out = 0;
    user.end_pass();

    return return_back;
}

void GuiEditor::deinit()
{
    debug_tree.deinit();
}
