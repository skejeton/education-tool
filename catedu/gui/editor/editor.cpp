#include "editor.hpp"
#include "catedu/misc/camera_input.hpp"
#include "catedu/rendering/render_model.hpp"
#include "catedu/ui/widgets.hpp"

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

void select_object(GuiEditor *editor, TableId id)
{
    editor->selection = id;
    editor->entity_list_page = id.id / 10;
}

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

    label(user, s, {1, 1});

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

    Vector4 bgc = {0.6f, 0.6f, 0.6f, 1.0f};
    if (is_selected)
    {
        bgc = {0.8f, 0.8f, 0.6f, 1.0f};
    }
    if (obj.hide)
    {
        bgc *= 0.5;
        bgc.w = 1.0f;
    }

    user.begin_generic(element, UiMakeBrush::make_solid(bgc),
                       UiMakeBrush::make_solid({0.0f, 0.0f, 0.0f, 1.0f}));

    if (button(user, "H"))
    {
        obj.hide = !obj.hide;
    }
    if (button(user, ">"))
    {
        result = Select;
    }
    if (button(user, "X"))
    {
        result = Delete;
    }
    show_object_icon(user, obj.type);
    label(user, obj.name, {1, 1.2},
          UiMakeBrush::make_solid({0.0f, 0.0f, 0.0f, 1.0f}));

    user.end_generic();

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
                       UiMakeBrush::make_solid({0.6f, 0.6f, 0.6f, 1.0f}),
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

bool GuiEditor::show(BoxdrawRenderer &renderer, ResourceSpec &resources,
                     Scene &scene, Input &input)
{
    bool return_back = false;
    debug_tree.reset();

    debug_tree.value("Mouse Pos X", input.mouse_pos.x);
    debug_tree.value("Mouse Pos Y", input.mouse_pos.y);
    debug_tree.value("Boolean value", false);

    scene.render(renderer, resources);

    camera.set_aspect(sapp_widthf() / sapp_heightf());
    camera_input_top_view_apply(&this->camera, &input);

    SelectionState sel =
        show_selection(*this, renderer, resources, scene, input);
    this->object_cursor_at = sel.position;
    if (sel.tilemap_selected && input.mouse_states[1].held)
    {
        sel.tilemap_selected->set_tile(vector2_to_vector2i(sel.position), 0);
    }
    if (sel.tilemap_selected && input.mouse_states[0].held)
    {
        sel.tilemap_selected->set_tile(vector2_to_vector2i(sel.position),
                                       this->tile_selection.id);
    }
    if (!sel.tilemap_selected && input.mouse_states[0].pressed)
    {
        if (this->selection != NULL_ID)
        {
            // Path: catedu/gui/editor/editor.cpp
            Object *selected = scene.get_object(this->selection);
            if (selected && selected->type == Object::Type::Entity)
            {
                selected->entity.pos =
                    this->object_cursor_at - Vector2{0.5, 0.5};
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

    boxdraw_flush(&renderer, this->camera.vp);

    UiUser user = UiUser::init(*this->ui_state);
    user.begin_pass();

    if (button(user, "Back"))
    {
        return_back = true;
    }

    char title[256];
    sprintf(title, "Objects | Page %zu", this->entity_list_page);

    begin_show_window(user, {title, {20, 20, 200, 410}});

    const size_t page_count =
        scene.objects.count > 0 ? (scene.objects.count - 1) / 10 : 0;
    this->entity_list_page =
        show_pagination(user, this->entity_list_page, page_count);

    size_t i = 0;

    for (auto [id, obj] : iter(scene.objects))
    {
        if (i++ < this->entity_list_page * 10)
        {
            continue;
        }
        if (i > (this->entity_list_page + 1) * 10)
        {
            break;
        }
        char idstr[256];
        sprintf(idstr, "%zu", id.id);

        user.state->element_storage.push(idstr, {});
        switch (show_object_row(user, obj, this->selection == id))
        {
        case Select:
            if (this->selection != id)
            {
                this->selection = id;
            }
            else
            {
                this->selection = {};
            }
            break;
        case Delete:
            scene.remove_object(id);
            if (this->selection == id)
            {
                this->selection = {};
            }
            break;
        default:
            break;
        }
        user.state->element_storage.pop();
    }

    end_show_window(user);

    Object *selected = scene.get_object(this->selection);

    if (input.key_states[SAPP_KEYCODE_TAB].held)
    {
        begin_show_window(user, {"Debug", {0, 0, 300, 400}});
        debug_tree.show(user);
        end_show_window(user);
    }

    begin_show_window(user, {"Help", {0, 0, 250, 400}});
    user.bold = true;
    label(user, "On object screen: ", {1.2, 1.2},
          UiMakeBrush::make_solid({0.0f, 1.0f, 1.0f, 1.0f}));
    user.bold = false;
    label(user, "`>` - Select object");
    label(user, "`X` - Delete object");
    label(user, "`H` - Hide object");
    user.bold = true;
    label(user, "There's different types of objects:", {1.2, 1.2},
          UiMakeBrush::make_solid({0.0f, 1.0f, 1.0f, 1.0f}));
    user.bold = false;
    label(user, "[T] - Tilemap");
    label(user, "[E] - Entity");
    label(user, "[B] - Backdrop");
    user.bold = true;
    label(user, "Controls:", {1.2, 1.2},
          UiMakeBrush::make_solid({0.0f, 1.0f, 1.0f, 1.0f}));
    user.bold = false;
    label(user, "WASD - Move camera");
    label(user, "Tab - Debug window");
    label(user, "Back - Go back");
    label(user, "Hold LMB on window to move it");
    label(user, "Press RMB on window to collapse it");
    label(user, "Press LMB on the scene to place objects");
    label(user, "Press Ctrl +/- to zoom in/out");
    user.bold = true;
    label(user, "Models:", {1.2, 1.2},
          UiMakeBrush::make_solid({0.0f, 1.0f, 1.0f, 1.0f}));
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
    end_show_window(user);

    if (selected)
    {
        begin_show_window(user, {"Properties", {220, 20, 200, 200}});

        show_object_icon_ex(user, selected->type);
        label(user, "Name:", {1, 1},
              UiMakeBrush::make_solid({0.0f, 0.0f, 0.0f, 1.0f}));
        ::input(user, "SelObjName", selected->name, 32);
        label(user, "Id:", {1, 1},
              UiMakeBrush::make_solid({0.0f, 0.0f, 0.0f, 1.0f}));
        ::input(user, "SelObjId", selected->id, 32);
        if (selected->type == Object::Type::Entity)
        {
            label(user, "Model:", {1, 1},
                  UiMakeBrush::make_solid({0.0f, 0.0f, 0.0f, 1.0f}));
            ::input(user, "SelObjModel", selected->entity.model_name, 32);
        }
        if (button(user, "OK"))
        {
            this->selection = NULL_ID;
        }

        end_show_window(user);

        if (selected->type == Object::Type::Tilemap)
        {
            begin_show_window(
                user,
                {"Tiles",
                 {sapp_widthf() / user.state->dpi_scale - 100, 20, 100, 800}});

            for (auto [id, tile] : iter(resources.tiles))
            {
                bool selected = this->tile_selection == id;
                Vector4 background = selected ? Vector4{0.8, 1.0, 0.8, 1.0}
                                              : Vector4{1.0, 1.0, 1.0, 1.0};

                if (button(user, tile.name, background))
                {
                    this->tile_selection = selected ? NULL_ID : id;
                }
            }

            end_show_window(user);
        }
    }
    if (this->placing_object)
    {
        Rect scr = sapp_screen_rect_scaled(this->ui_state->dpi_scale);
        Rect rect = rect_center_rect(scr, {0, 0, 100, 200});
        begin_show_window(user, {"Place object", rect});
        label(user, "Type:");
        if (button(user, "Entity"))
        {
            Object new_obj = {};
            new_obj.type = Object::Type::Entity;
            strcpy(
                new_obj.name,
                stdstrfmt("Unnamed Entity %zu", scene.objects.count).c_str());
            new_obj.entity =
                ObjEntity::init("", this->object_cursor_at - Vector2{0.5, 0.5});

            select_object(this, scene.add_object(new_obj));
            this->placing_object = false;
        }
        if (button(user, "Tilemap"))
        {
            Object new_obj = {};
            new_obj.type = Object::Type::Tilemap;
            new_obj.tilemap = ObjTilemap::init();
            strcpy(
                new_obj.name,
                stdstrfmt("Unnamed Tilemap %zu", scene.objects.count).c_str());

            select_object(this, scene.add_object(new_obj));
            this->placing_object = false;
        }
        if (button(user, "Backdrop"))
        {
            Object new_obj = {};
            new_obj.type = Object::Type::Backdrop;
            new_obj.backdrop = ObjBackdrop::init({0, 32, 32, 32});
            strcpy(
                new_obj.name,
                stdstrfmt("Unnamed Backdrop %zu", scene.objects.count).c_str());

            select_object(this, scene.add_object(new_obj));
            this->placing_object = false;
        }
        if (button(user, "Cancel"))
        {
            this->placing_object = false;
        }

        end_show_window(user);
    }

    user.end_pass();

    return return_back;
}

void GuiEditor::deinit()
{
    debug_tree.deinit();
    // Nothing yet
}
