#include "widgets.hpp"
#include "catedu/sys/oom.hpp"
#include "resources/load_image.hpp"

static const Vector4 theme[] = {{0.8, 0.8, 0.8, 1.0}, {0.6, 0.6, 0.6, 1.0},
                                {0.0, 0.0, 0.0, 1.0}, {0.0, 0.0, 0.0, 1.0},
                                {0.7, 0.7, 0.7, 1.0}, {0.5, 0.5, 0.5, 1.0}};

void begin_show_window(UiUser &user, WindowInfo info)
{
    AutoLayoutElement cel = {};
    cel.position = AutoLayoutPosition::Absolute;
    cel.offset = info.rect.pos;
    cel.width.type = AutoLayoutDimension::Auto;
    cel.height.type = AutoLayoutDimension::Auto;
    cel.layout.type = AutoLayout::Column;
    user.begin_generic(cel, {}, {});

    // Titlebar
    {
        AutoLayoutElement el = {};
        el.border = {1, 1, 1, 1};
        el.width.type = AutoLayoutDimension::Pixel;
        el.width.value = info.rect.siz.x;
        el.height.type = AutoLayoutDimension::Auto;

        UiBrush border = UiMakeBrush::make_solid({0.7f, 0.7f, 0.7f, 1.0f});
        UiBrush background = UiMakeBrush::make_solid({0.0f, 0.0f, 0.5f, 1.0f});
        user.begin_generic(el, background, border);
        label(user, info.title, {1, 1});
        user.end_generic();
    }

    // Content
    {
        AutoLayoutElement el = {};
        el.border = {1, 1, 1, 1};
        el.width.type = AutoLayoutDimension::Pixel;
        el.width.value = info.rect.siz.x;
        el.height.type = AutoLayoutDimension::Pixel;
        el.height.value = info.rect.siz.y;

        UiBrush border = UiMakeBrush::make_solid({0.7f, 0.7f, 0.7f, 1.0f});
        UiBrush background = UiMakeBrush::make_solid({0.6f, 0.6f, 0.6f, 1.0f});
        user.begin_generic(el, background, border);
    }
}

void end_show_window(UiUser &user)
{
    user.end_generic();
    user.end_generic();
}

bool button(UiUser &user, const char *text, int offs)
{
    user.state->element_storage.push(text, {});
    UiPersistentElement *pe = user.state->element_storage.value();

    AutoLayoutElement el = {};
    el.layout = {AutoLayout::Row};
    el.width = {AutoLayoutDimension::Auto};
    el.height = {AutoLayoutDimension::Auto};
    el.padding = {3, 3, 3, 3};
    el.margin = {1, 1, 1, 1};
    el.border = {1, 1, 1, 1};
    el.position = AutoLayoutPosition::Relative;
    el.offset = {(float)offs, (float)-offs};

    Vector4 color_top = theme[1];
    Vector4 color_bottom = theme[0];

    bool pressed = false;

    if (user.state->interaction_table.hovered ==
        user.state->element_storage.id())
    {
        color_top = theme[5];
        color_bottom = theme[4];
        if (user.state->input.mouse_down)
        {
            std::swap(color_top, color_bottom);
        }
        pressed = user.state->input.mouse_pressed;
    }

    user.begin_generic(el, UiMakeBrush::make_gradient(color_bottom, color_top),
                       UiMakeBrush::make_solid(theme[2]),
                       user.state->element_storage.id());

    label(user, text, {1, 1}, UiMakeBrush::make_solid(theme[3]));

    user.end_generic();
    user.state->element_storage.pop();

    return pressed;
}

void img(UiUser &user, const char *path, Vector2 scale)
{
    UiImageId id = NULL_ID;
    for (auto [i, img] : iter(user.state->image_cache))
    {
        if (strcmp(img.path, path) == 0)
        {
            id = img.id;
            break;
        }
    }

    if (id == NULL_ID)
    {
        id = ui_resources_load_image(user.state->core, path);

        char *newpath =
            strcpy((char *)OOM_HANDLER(malloc(strlen(path) + 1)), path);

        user.state->image_cache.allocate({newpath, id});
    }

    AutoLayoutElement el = {};
    el.layout = {AutoLayout::Row};
    Vector2 size = vector2i_to_vector2(user.state->core->get_image(id)->size);

    el.width = {AutoLayoutDimension::Pixel, size.x * scale.x};
    el.height = {AutoLayoutDimension::Pixel, size.y * scale.y};

    user.begin_generic(el,
                       UiMakeBrush::make_image_brush(UiBuffers::Rectangle,
                                                     user.state->core, id)
                           .build(),
                       UiMakeBrush::make_solid({1.0, 1.0, 1.0, 0.0}));

    user.end_generic();
}

void label(UiUser &user, const char *text, Vector2 scale, UiBrush style)
{
    Vector2 size = user.state->font.bounds_text_utf8({0, 0}, text, scale).siz;

    void *ptr = user.bump.alloc(strlen(text) + 1);
    strcpy((char *)ptr, text);

    AutoLayoutElement el = {};
    el.layout = {AutoLayout::Row};
    el.width = {AutoLayoutDimension::Pixel, size.x};
    el.height = {AutoLayoutDimension::Pixel, size.y};
    el.userdata =
        user.styles.allocate(UiGenericStyles{style, {}, (char *)ptr, scale});

    user.layout.add_element(user.current_node, el);
}

void input(UiUser &user, const char *id, char *out, int max)
{
    user.state->element_storage.push(id, {true});
    UiPersistentElement *pe = user.state->element_storage.value();

    bool focused = user.state->interaction_table.focused ==
                   user.state->element_storage.id();
    if (focused)
    {
        for (int i = 0; i < user.state->input.inputchars_count; i++)
        {
            if (user.state->input.inputchars[i] == '\b')
            {
                if (strlen(out) > 0)
                    out[strlen(out) - 1] = 0;
            }
            else if (user.state->input.inputchars[i])
            {
                if (strlen(out) < max - 1)
                {
                    out[strlen(out)] = user.state->input.inputchars[i];
                }
            }
        }
    }

    AutoLayoutElement el = {};
    el.layout = {AutoLayout::Row};
    el.width = {AutoLayoutDimension::Auto};
    el.height = {AutoLayoutDimension::Auto};
    el.padding = {2, 2, 2, 2};
    el.margin = {0, 0, 0, 0};
    el.border = {1, 1, 1, 1};

    user.begin_generic(el, UiMakeBrush::make_solid(theme[0]),
                       focused ? UiMakeBrush::make_solid(theme[3])
                               : UiMakeBrush::make_solid(theme[1]),
                       user.state->element_storage.id());

    char *t = (char *)OOM_HANDLER(malloc(max + 3));
    memcpy(t, out, strlen(out));
    t[strlen(out)] = '|';
    t[strlen(out) + 1] = 0;

    label(user, t, {1.2, 1.2}, UiMakeBrush::make_solid(theme[2]));
    free(t);
    user.end_generic();

    user.state->element_storage.pop();
}
