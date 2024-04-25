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

    Vector4 bottom = {color.x * 1.25f, color.y * 1.25f, color.z * 1.25f,
                      color.w};
    color.w *= 0.4f;

    user.begin_generic(element, UiMakeBrush::make_gradient(bottom, color),
                       UiMakeBrush::make_gradient({0.0f, 0.0f, 0.0f, 0.5f},
                                                  {0.0f, 0.0f, 0.0f, 0.0f}));

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

    Vector4 bgc = {1.0f, 1.0f, 1.0f, 1.0f};
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

    Vector4 color_bottom = {bgc.x * 1.25f, bgc.y * 1.25f, bgc.z * 1.25f, bgc.w};
    bgc.w *= 0.4f;

    user.begin_generic(element, UiMakeBrush::make_gradient(color_bottom, bgc),
                       UiMakeBrush::make_gradient({0.0f, 0.0f, 0.0f, 0.5f},
                                                  {0.0f, 0.0f, 0.0f, 0.0f}),
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
                       UiMakeBrush::make_solid({1.0f, 1.0f, 1.0f, 1.0f}),
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
    editor.dialog_editor.selected = -1;
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
    label(user, "F3 - Debug window");
    label(user, "Ctrl + Z - Undo");
    label(user, "Ctrl + Y - Redo");
    label(user, "Ctrl + S - Save");
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
        select_object(editor, NULL_ID);
    }

    end_show_window(user);
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

void begin_toolbar(UiUser &user, const char *name, bool left)
{
    float align_x = left ? 0 : 1;
    float align_y = 0.5;

    AutoLayoutElement element = create_main_element(user);
    element.align_height = align_y;
    element.align_width = align_x;
    user.begin_generic(element, {}, {});
    user.push_id(name);

    AutoLayoutElement toolbar = {};
    toolbar.layout.type = AutoLayout::Column;
    toolbar.width.type = AutoLayoutDimension::Auto;
    toolbar.height.type = AutoLayoutDimension::Auto;
    toolbar.padding = {2, 2, 2, 2};
    toolbar.margin = {2, 2, 2, 2};
    toolbar.border = {1, 1, 1, 1};
    user.begin_generic(toolbar, UiMakeBrush::make_solid({0.8, 0.8, 0.8, 1.0}),
                       UiMakeBrush::make_solid({0.0, 0.0, 0.0, 1.0}),
                       user.state->element_storage.id());
}

void end_toolbar(UiUser &user)
{
    user.end_generic();
    user.pop_id();
    user.end_generic();
}

bool icon_button(UiUser &user, const char *name, const char *icon,
                 Vector4 color)
{
    AutoLayoutElement el = {};
    el.padding = {2, 2, 2, 2};
    el.margin = {2, 2, 2, 2};
    el.border = {1, 1, 1, 1};

    begin_button_frame(user, name, el, color);
    img(user, icon, {1, 1});
    return end_button_frame(user);
}

bool icon_replacement_button(UiUser &user, const char *name, Vector4 color)
{
    AutoLayoutElement el = {};
    el.width = {AutoLayoutDimension::Pixel, 64};
    el.height = {AutoLayoutDimension::Pixel, 64};
    el.align_width = 0.5;
    el.align_height = 0.5;
    el.padding = {2, 2, 2, 2};
    el.margin = {2, 2, 2, 2};
    el.border = {1, 1, 1, 1};

    begin_button_frame(user, name, el, color);
    label(user, name, {1, 1});
    return end_button_frame(user);
}

void show_stencil_picker(UiUser &user, StencilEdit &stencil)
{
    begin_toolbar(user, "Stencil", true);

    auto stencil_button = [&](const char *name, const char *img,
                              StencilType type) {
        Vector4 color = stencil.type == type ? Vector4{0.8, 1.0, 0.8, 1.0}
                                             : Vector4{1.0, 1.0, 1.0, 1.0};
        if (icon_button(user, name, img, color))
        {
            stencil.type = type;
        }
    };

    stencil_button("Freeform", "assets/gui/freeform.png",
                   StencilType::Freeform);
    stencil_button("Rectangle", "assets/gui/rectangle.png",
                   StencilType::Rectangle);
    stencil_button("Line", "assets/gui/line.png", StencilType::Line);
    stencil_button("LineRectangle", "assets/gui/line_rectangle.png",
                   StencilType::LineRectangle);
    stencil_button("Ellipse", "assets/gui/ellipse.png", StencilType::Ellipse);

    end_toolbar(user);
}

void show_tile_picker(UiUser &user, ResourceSpec &resources, TilemapEdit &edit)
{
    begin_toolbar(user, "Tiles", false);

    int limit = 5;

    if (icon_replacement_button(user, "^", {1.0, 1.0, 1.0, 1.0}))
    {
        edit.scroll -= 1;
    }

    edit.scroll = clamp(edit.scroll, 0, (int)resources.tiles.count - limit);

    int i = 0;
    for (auto [id, tile] : iter(resources.tiles))
    {
        if (i < edit.scroll || i >= edit.scroll + limit)
        {
            i++;
            continue;
        }

        bool selected = edit.tile == id;
        Vector4 background = selected ? Vector4{0.8, 1.0, 0.8, 1.0}
                                      : Vector4{1.0, 1.0, 1.0, 1.0};

        if (icon_replacement_button(user, tile.name, background))
        {
            edit.tile = selected ? NULL_ID : id;
        }

        i++;
    }

    if (icon_replacement_button(user, "v", {1.0, 1.0, 1.0, 1.0}))
    {
        edit.scroll += 1;
    }

    Vector4 background = edit.tile == NULL_ID ? Vector4{0.8, 1.0, 0.8, 1.0}
                                              : Vector4{1.0, 1.0, 1.0, 1.0};
    if (icon_replacement_button(user, "x", background))
    {
        edit.tile = NULL_ID;
    }

    end_toolbar(user);
}

void apply_stencil(GuiEditor &editor, StencilEdit &edit, TableId tile_id,
                   TableId tilemap_entity, Scene &scene)
{
    edit.map([&](int x, int y) {
        EditAction action = {};
        action.type = EditAction::PlaceTile;
        action.cmd.place_tile.tilemap_entity = tilemap_entity;
        action.cmd.place_tile.pos = {x, y};
        action.cmd.place_tile.tile_id = tile_id.id;

        do_action(editor, scene, action);
    });
}

void show_stencil(StencilEdit &edit, TableId model, ResourceSpec &resources,
                  catedu::pbr::Renderer &renderer)
{
    if (model == NULL_ID)
    {
        return;
    }

    edit.map([&](int x, int y) {
        Vector3 pos = {x, 0, y};
        render_model_at(pos, resources, model, renderer, true, true);
    });
}

void show_stencil_editor(Input &input, GuiEditor &editor, StencilEdit &edit,
                         TableId tile_id, ResourceSpec &resources,
                         catedu::pbr::Renderer &renderer, Scene &scene)
{
    if (input.k[INPUT_MB_LEFT].pressed)
    {
        edit.start = vector2_to_vector2i(editor.object_cursor_at);
    }
    else if (input.k[INPUT_MB_LEFT].held)
    {
        edit.end = vector2_to_vector2i(editor.object_cursor_at);

        SpecTile *tile = resources.tiles.get(tile_id);
        if (tile == nullptr)
        {
            show_stencil(edit, resources.find_model_by_name("selector"),
                         resources, renderer);
        }
        else
        {
            show_stencil(edit, tile->model_id, resources, renderer);
        }

        if (edit.type == StencilType::Freeform)
        {
            apply_stencil(editor, edit, tile_id, editor.selection, scene);
            edit.start = edit.end;
        }
    }

    if (input.k[INPUT_MB_LEFT].released)
    {
        apply_stencil(editor, edit, tile_id, editor.selection, scene);
        edit.end = edit.start = {};
    }
}

void show_tile_editor(UiUser &user, ResourceSpec &resources, GuiEditor &editor)
{
    show_stencil_picker(user, editor.tilemap_edit.stencil);
    show_tile_picker(user, resources, editor.tilemap_edit);
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
    result.dialog_editor = DialogEditor::init();

    return result;
}

struct SelectionState
{
    ObjTilemap *tilemap_selected;
    Vector2 position;
};

SelectionState show_selection(GuiEditor &editor,
                              catedu::pbr::Renderer &renderer,
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

void GuiEditor::start_playtest(Scene &scene, ResourceSpec &resources,
                               bool *reload_module)
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
            this->conversation_stage = 0;
        }
    }
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

bool GuiEditor::show_old_mode(UiUser &user, catedu::pbr::Renderer &renderer,
                              ResourceSpec &resources, Scene &scene, void *umka,
                              bool *reload_module)
{
#ifdef RUNTIME_MODE
    if (!this->playtesting)
    {
        start_playtest(scene, resources, reload_module);
    }
#endif
    renderer.begin_pass();

    renderer.camera = camera;

    Input &input = this->ui_state->input;

    if (input.k[SAPP_KEYCODE_F3].pressed)
    {
        this->show_debug = !this->show_debug;
    }

    AutoLayoutElement element = create_main_element(user);
    user.state->element_storage.push("Main", {});
    user.begin_generic(element, {}, {}, user.state->element_storage.id());

    bool return_back = false;
    bool no_ui = false;

    debug_tree.reset();

    debug_tree.value("Mouse Pos X", input.mouse_pos.x);
    debug_tree.value("Mouse Pos Y", input.mouse_pos.y);
    debug_tree.value("Dirty", dirty);

    if (this->playtesting)
    {
        this->playtest_scene->render(renderer, resources, this->show_debug);
    }
    else
    {
        handle_camera_movement(camera, input, user);

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
            if (sel.tilemap_selected)
            {
                show_stencil_editor(input, *this, this->tilemap_edit.stencil,
                                    this->tilemap_edit.tile, resources,
                                    renderer, scene);
            }

            this->object_cursor_at = sel.position;

            if (input.k[INPUT_MB_LEFT].pressed && user.hovered() &&
                !sel.tilemap_selected)
            {
                ObjectId closest_object =
                    find_object_within_distance(scene, object_cursor_at, 1);

                if (closest_object != NULL_ID)
                {
                    this->selection = closest_object;
                }
                else
                {
                    if (this->selection != NULL_ID)
                    {
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

    renderer.end_pass();

    user.end_generic();
    this->ui_state->element_storage.pop();

    if (no_ui)
    {
        return return_back;
    }

#ifndef RUNTIME_MODE
    if (button(user, "Back"))
    {
        return_back = true;
    }
#endif

    if (this->playtesting)
    {
        UmkaStackSlot empty;
        int func = umkaGetFunc(umka, NULL, "onUI");
        if (UmkaError *error = umkaGetError(umka); error->code != 0)
        {
            if (!this->suppress_errors)
            {
                const char *buttons[] = {"Retry", "Ignore", NULL};

                switch (msgbox(user, "Umka Error",
                               stdstrfmt("%s\n    at %s:%d", error->msg,
                                         error->fileName, error->line)
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

#ifndef RUNTIME_MODE
    if (button(user, this->playtesting ? "Exit playtest" : "Playtest"))
    {
        if (this->playtesting)
        {
            if (this->playtest_scene)
            {
                this->playtest_scene->deinit();
                free(this->playtest_scene);
            }
            this->dialog = nullptr;
            this->playtesting = false;
        }
        else
        {
            start_playtest(scene, resources, reload_module);
        }
    }
#endif

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

        if (begin_show_window(user, {"Help", {0, 0, 250, 500}}))
        {
            show_help(user, resources);
        }
        end_show_window(user);

        if (selected)
        {
            show_properties(user, *selected, *this);

            if (selected->type == Object::Type::Entity)
            {
                this->dialog_editor.show(user, selected->entity.dialog);
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

            const char *source = "";

            ObjectId coll_id = find_object_within_distance_not_player(
                *this->playtest_scene, obj->entity.pos, 3);

            DialogList *dialog = nullptr;
            if (coll_id != NULL_ID)
            {
                Object *coll_obj = this->playtest_scene->get_object(coll_id);
                if (coll_obj->type == Object::Type::Entity)
                {
                    dialog = &coll_obj->entity.dialog;
                    source = coll_obj->id;
                }
            }

            int func = umkaGetFunc(umka, NULL, "onInteract");
            assert(func);
            UmkaStackSlot id;

            id.ptrVal = umkaMakeStr(umka, (char *)source);

            if (umkaGetError(umka)->code == 0)
            {
                umkaCall(umka, func, 1, &id, NULL);
            }

            if (dialog)
            {
                this->dialog = dialog;
                this->conversation_stage = 0;
            }
        }

        this->playtest_scene->update(resources);
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

    if (this->playtesting && this->dialog)
    {
        int i = 0;
        for (auto dialog : iter(this->dialog->dialogs))
        {
            if (conversation_stage == i)
            {
                const char *options[] = {"Next", NULL};
                switch (msgbox(user, dialog.name, dialog.text, MsgBoxType::Info,
                               options))
                {
                case 0:
                    this->conversation_stage++;
                    goto end;
                }
            }
            i++;
        }
    end:;
    }

    if (*reload_module)
    {
        this->suppress_errors = false;
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
}

void show_popups(UiUser &user, GuiEditor &editor)
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
        case 1:
            editor.exit_requested = false;
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

bool show_new_mode(GuiEditor &editor, UiUser &user, ResourceSpec &resources,
                   catedu::pbr::Renderer &renderer, Scene &scene, Input &input)
{
    AutoLayoutElement element = create_main_element(user);
    user.state->element_storage.push("Main", {});
    user.begin_generic(element, {}, {}, user.state->element_storage.id());

    renderer.camera = editor.camera;
    renderer.begin_pass();

    scene.render(renderer, resources);

    if (user.hovered())
    {
        if (!handle_camera_movement(editor.camera, input, user))
        {
            SelectionState sel =
                show_selection(editor, renderer, resources, scene, input);

            editor.object_cursor_at = sel.position;

            if (sel.tilemap_selected)
            {
                show_stencil_editor(input, editor, editor.tilemap_edit.stencil,
                                    editor.tilemap_edit.tile, resources,
                                    renderer, scene);
            }
        }
    }

    bool tilemap_selected =
        editor.selection != NULL_ID &&
        scene.get_object(editor.selection)->type == Object::Type::Tilemap;

    if (tilemap_selected)
    {
        show_tile_editor(user, resources, editor);
    }

    renderer.end_pass();

    user.end_generic();
    user.state->element_storage.pop();

    return false;
}

bool GuiEditor::show(catedu::pbr::Renderer &renderer, ResourceSpec &resources,
                     Scene &scene, UiUser &user, void *umka,
                     bool *reload_module)
{
    if (user.state->input.k[SAPP_KEYCODE_F5].pressed)
    {
        this->new_mode = !this->new_mode;
    }

    bool return_back = false;

    if (new_mode)
    {
        return_back = show_new_mode(*this, user, resources, renderer, scene,
                                    user.state->input);
    }
    else
    {
        return_back = show_old_mode(user, renderer, resources, scene, umka,
                                    reload_module);
    }

    show_popups(user, *this);

    return return_back;
}

void GuiEditor::deinit()
{
    debug_tree.deinit();
    if (this->playtesting)
    {
        this->playtest_scene->deinit();
        free(this->playtest_scene);
    }
}
