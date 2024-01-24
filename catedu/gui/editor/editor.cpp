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

    user.label(s, {1, 1});

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

    if (user.button("H"))
    {
        obj.hide = !obj.hide;
    }
    if (user.button(">"))
    {
        result = Select;
    }
    if (user.button("X"))
    {
        result = Delete;
    }
    show_object_icon(user, obj.type);
    user.label(obj.name, {1, 1.2},
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

    if (user.button(" First "))
    {
        page = 0;
    }
    if (user.button(" < ") && page != 0)
    {
        page -= 1;
    }
    if (user.button(" > "))
    {
        page += 1;
    }
    if (user.button(" Last "))
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

void GuiEditor::show(BoxdrawRenderer &renderer, ResourceSpec &resources,
                     Scene &scene, Input &input)
{
    scene.render(renderer, resources);

    float window_width = sapp_widthf(), window_height = sapp_heightf();
    float aspect = window_width / window_height;

    camera.set_aspect(aspect);
    camera_input_top_view_apply(&this->camera, &input);

    float distance;
    Ray3 ray = this->camera.screen_to_world_ray(
        input.mouse_pos, Vector2{window_width, window_height});

    if (ray3_vs_horizontal_plane(ray, -0.5, &distance))
    {
        TableId model_id = resources.find_model_by_name("pointer");

        render_model_at(ray3_at(ray, distance), resources, model_id, renderer,
                        true);
    }

    UiUser user = UiUser::init(*this->ui_state);
    user.begin_pass();

    char title[256];
    sprintf(title, "Objects | Page %zu", this->entity_list_page);

    begin_show_window(user, {title, {20, 50, 200, 410}});

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

        user.state->elements.push(idstr, {});
        switch (show_object_row(user, obj, this->selection == id))
        {
        case Select:
            this->selection = id;
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
        user.state->elements.pop();
    }

    end_show_window(user);

    Object *selected = scene.get_object(this->selection);

    if (selected)
    {
        begin_show_window(user, {"Properties", {220, 20, 200, 200}});
        show_object_icon_ex(user, selected->type);
        user.label("Name:", {1, 1.2},
                   UiMakeBrush::make_solid({0.0f, 0.0f, 0.0f, 1.0f}));
        user.input("SelObjName", selected->name, 32);
        user.label("Id:", {1, 1.2},
                   UiMakeBrush::make_solid({0.0f, 0.0f, 0.0f, 1.0f}));
        user.input("SelObjId", selected->id, 32);
        end_show_window(user);
    }

    user.end_pass();
}

void GuiEditor::deinit()
{
    // Nothing yet
}
