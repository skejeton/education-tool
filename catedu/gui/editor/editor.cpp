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

    if (ray3_vs_horizontal_plane(ray, 0.0, &distance))
    {
        TableId model_id = resources.find_model_by_name("selector");
        Vector3 pos3d = ray3_at(ray, distance);
        pos = {pos3d.x, pos3d.z};

        Object *selected = scene.get_object(editor.selection);
        if (selected)
        {
            if (selected->type == Object::Type::Tilemap)
            {
                pos.x = round(pos.x);
                pos.y = round(pos.y);

                tilemap_selected = &selected->tilemap;
            }
        }

        pos3d.x = pos.x;
        pos3d.z = pos.y;

        render_model_at(pos3d, resources, model_id, renderer, true);
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
    if (sel.tilemap_selected && input.mouse_states[1].held)
    {
        sel.tilemap_selected->set_tile(vector2_to_vector2i(sel.position), 0);
    }
    if (sel.tilemap_selected && input.mouse_states[0].held)
    {
        sel.tilemap_selected->set_tile(vector2_to_vector2i(sel.position),
                                       this->tile_selection.id);
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
        end_show_window(user);

        if (selected->type == Object::Type::Tilemap)
        {
            begin_show_window(
                user,
                {"Tiles",
                 {sapp_widthf() / user.state->dpi_scale - 100, 20, 100, 800}});

            for (auto [id, tile] : iter(resources.tiles))
            {
                if (button(user, tile.name))
                {
                    this->tile_selection = id;
                }
            }

            end_show_window(user);
        }
    }

    user.end_pass();

    return return_back;
}

void GuiEditor::deinit()
{
    debug_tree.deinit();
    // Nothing yet
}
