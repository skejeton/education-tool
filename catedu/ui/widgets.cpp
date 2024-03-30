#include "widgets.hpp"
#include "catedu/sys/oom.hpp"
#include "resources/load_image.hpp"

static const Vector4 theme[] = {
    {0.9, 0.9, 1.0, 1.0}, {0.7, 0.7, 0.8, 1.0}, // Button
    {0.1, 0.1, 0.2, 1.0},                       // Text
    {0.1, 0.1, 0.2, 1.0},                       // Border
    {0.7, 0.7, 0.9, 1.0}, {0.6, 0.6, 0.7, 1.0}, // Button Pressed
    {1.0, 1.0, 1.0, 1.0}                        // Window
};

bool begin_show_window(UiUser &user, WindowInfo info)
{
    user.state->element_storage.push(stdstrfmt("%s#window", info.title).c_str(),
                                     {false, {}, {}, info.rect});
    user.state->element_storage.element_retainer.bump(9999999);

    UiPersistentElement *pe = user.state->element_storage.value();

    if (user.hovered() && user.state->input.k[INPUT_MB_LEFT].pressed)
    {
        pe->pin = user.state->input.mouse_pos - pe->persistent_box.pos;
        pe->pinned = true;
    }
    if (user.hovered() && user.state->input.k[INPUT_MB_RIGHT].pressed)
    {
        pe->hidden = !pe->hidden;
    }
    if (pe->pinned && user.state->input.k[INPUT_MB_LEFT].held)
    {
        pe->persistent_box.pos +=
            ((user.state->input.mouse_pos - pe->persistent_box.pos) - pe->pin) /
            user.state->dpi_scale;
        pe->pin = user.state->input.mouse_pos - pe->persistent_box.pos;
    }
    else
    {
        pe->pinned = false;
    }

    pe->persistent_box.siz.x = std::max(pe->persistent_box.siz.x, 100.0f);
    pe->persistent_box.siz.y = std::max(pe->persistent_box.siz.y, 100.0f);

    if (!info.if_static)
    {
        info.rect = pe->persistent_box;
    }

    AutoLayoutElement cel = {};
    cel.position = AutoLayoutPosition::Absolute;
    cel.offset = info.rect.pos;
    cel.width.type = AutoLayoutDimension::Auto;
    cel.height.type = AutoLayoutDimension::Auto;
    cel.layout.type = AutoLayout::Column;
    cel.border = {1, 1, 2, 1};
    user.begin_generic(cel, {},
                       UiMakeBrush::make_gradient({0.0f, 0.0f, 0.0f, 0.3f},
                                                  {0.0f, 0.0f, 0.0f, 0.0f}),
                       user.state->element_storage.id());

    // Titlebar
    {
        AutoLayoutElement el = {};
        el.border = {3, 3, 3, 3};
        el.width.type = AutoLayoutDimension::Pixel;
        el.width.value = info.rect.siz.x - 6;
        el.height.type = AutoLayoutDimension::Auto;
        el.padding = {3, 3, 3, 3};
        el.clip = true;

        UiBrush border = UiMakeBrush::make_gradient(
            theme[0] * Vector4{1, 1, 1, 0.9}, theme[1] * Vector4{1, 1, 1, 0.7});
        UiBrush background = UiMakeBrush::make_solid({0.0f, 0.0f, 0.5f, 0.0f});
        user.begin_generic(el, background, border);
        label(user, info.title, {1, 1});
        user.end_generic();
    }

    // Content
    {
        AutoLayoutElement el = {};
        el.border = {3, 3, 3, 3};
        el.width.type = AutoLayoutDimension::Pixel;
        el.width.value = info.rect.siz.x;
        el.height.type = AutoLayoutDimension::Pixel;
        el.height.value = info.rect.siz.y;
        el.hidden = pe->hidden;
        el.clip = true;

        UiBrush border = UiMakeBrush::make_solid(theme[0]);
        UiBrush background = UiMakeBrush::make_solid(theme[6]);
        user.begin_generic(el, background, border);
    }

    // Resize handle
    {
        AutoLayoutElement el = {};
        el.width.type = AutoLayoutDimension::Pixel;
        el.width.value = 6;
        el.height.type = AutoLayoutDimension::Pixel;
        el.height.value = 6;
        el.clip = true;
        el.position = AutoLayoutPosition::Detached;
        el.offset = info.rect.siz;
        el.pop = true;

        user.state->element_storage.push(".resize", {true, {}, {}, {}});
        user.begin_generic(el, {}, {}, user.state->element_storage.id());

        if (user.active() && user.state->input.k[INPUT_MB_LEFT].held)
        {
            pe->persistent_box.siz +=
                user.state->input.mouse_delta / user.state->dpi_scale;
        }

        user.end_generic();
        user.state->element_storage.pop();
    }

    // Resize handle bottom
    {
        AutoLayoutElement el = {};
        el.width.type = AutoLayoutDimension::Pixel;
        el.width.value = info.rect.siz.x;
        el.height.type = AutoLayoutDimension::Pixel;
        el.height.value = 6;
        el.clip = true;
        el.position = AutoLayoutPosition::Detached;
        el.offset = {0, info.rect.siz.y};
        el.pop = true;

        user.state->element_storage.push(".resizev", {true, {}, {}, {}});
        user.begin_generic(el, {}, {}, user.state->element_storage.id());

        if (user.active() && user.state->input.k[INPUT_MB_LEFT].held)
        {
            pe->persistent_box.siz.y +=
                user.state->input.mouse_delta.y / user.state->dpi_scale;
        }

        user.end_generic();
        user.state->element_storage.pop();
    }

    // Resize handle right
    {
        AutoLayoutElement el = {};
        el.width.type = AutoLayoutDimension::Pixel;
        el.width.value = 6;
        el.height.type = AutoLayoutDimension::Pixel;
        el.height.value = info.rect.siz.y;
        el.clip = true;
        el.position = AutoLayoutPosition::Detached;
        el.offset = {info.rect.siz.x, 0};
        el.pop = true;

        user.state->element_storage.push(".resizeh", {true, {}, {}, {}});
        user.begin_generic(el, {}, {}, user.state->element_storage.id());

        if (user.active() && user.state->input.k[INPUT_MB_LEFT].held)
        {
            pe->persistent_box.siz.x +=
                user.state->input.mouse_delta.x / user.state->dpi_scale;
        }

        user.end_generic();
        user.state->element_storage.pop();
    }

    return !pe->hidden;
}

void end_show_window(UiUser &user)
{
    user.end_generic();
    user.end_generic();

    user.state->element_storage.pop();
}

bool button(UiUser &user, const char *text, Vector4 background)
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

    Vector4 color_top = theme[1] * background;
    Vector4 color_bottom = theme[0] * background;

    bool pressed = false;

    if (user.hovered())
    {
        color_top = theme[5] * background;
        color_bottom = theme[4] * background;
        pressed = user.state->input.k[INPUT_MB_LEFT].released;
    }

    if (user.state->input.k[INPUT_MB_LEFT].held && user.active())
    {
        std::swap(color_top, color_bottom);
    }

    color_top.w = 0.4f;

    user.begin_generic(el, UiMakeBrush::make_gradient(color_bottom, color_top),
                       UiMakeBrush::make_gradient({0.0f, 0.0f, 0.0f, 0.5f},
                                                  {0.0f, 0.0f, 0.0f, 0.0f}),
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
    if (user.bold)
    {
        size = user.state->font_bold.bounds_text_utf8({0, 0}, text, scale).siz;
    }

    void *ptr = user.bump.alloc(strlen(text) + 1);
    strcpy((char *)ptr, text);

    AutoLayoutElement el = {};
    el.layout = {AutoLayout::Row};
    el.width = {AutoLayoutDimension::Pixel, size.x};
    el.height = {AutoLayoutDimension::Pixel, size.y};

    UiGenericStyles styles = {style, {}, (char *)ptr, scale, 0};
    styles.bold = user.bold;

    el.userdata = user.styles.allocate(styles);

    user.layout.add_element(user.current_node, el);
}

void input(UiUser &user, const char *id, char *out, int max)
{
    user.state->element_storage.push(id, {true});
    UiPersistentElement *pe = user.state->element_storage.value();

    if (user.focused())
    {
        for (int i = 0; i < user.state->input.input_len; i++)
        {
            if (user.state->input.input[i] == '\b')
            {
                if (strlen(out) > 0)
                    out[strlen(out) - 1] = 0;
            }
            else if (user.state->input.input[i])
            {
                if (strlen(out) < max - 1)
                {
                    out[strlen(out)] = user.state->input.input[i];
                }
            }
        }
    }

    AutoLayoutElement el = {};
    el.layout = {AutoLayout::Row};
    el.width = {AutoLayoutDimension::Pixel, 150};
    el.height = {AutoLayoutDimension::Auto};
    el.clip = true;
    el.padding = {2, 2, 2, 2};
    el.margin = {0, 0, 0, 0};
    el.border = {1, 1, 1, 1};

    UiBrush bg = UiMakeBrush::make_gradient(theme[0], theme[1]);
    bg.color_top.w = 0.4;
    UiBrush border = UiMakeBrush::make_gradient({0.0f, 0.0f, 0.0f, 0.5f},
                                                {0.0f, 0.0f, 0.0f, 0.0f});

    if (user.focused())
    {
        bg = UiMakeBrush::make_gradient(theme[5], theme[4]);
        bg.color_top.w = 0.4;
        el.width = {AutoLayoutDimension::Auto};
        el.pop = true;
    }

    user.begin_generic(el, bg, border, user.state->element_storage.id());

    char *t = (char *)OOM_HANDLER(malloc(max + 3));
    memcpy(t, out, strlen(out));
    t[strlen(out)] = user.focused() ? '|' : 0;
    t[strlen(out) + 1] = 0;

    label(user, t, {1.2, 1.2}, UiMakeBrush::make_solid(theme[2]));

    free(t);
    user.end_generic();

    user.state->element_storage.pop();
}

AutoLayoutElement make_element(AutoLayout layout, Vector2 size, bool autox,
                               bool autoy, Vector2 align = {0, 0}, float p = 3);

AutoLayoutElement make_auto(AutoLayout layout, Vector2 align = {0, 0});

int msgbox(UiUser &user, const char *title, const char *text, MsgBoxType type,
           const char *buttons[])
{
    begin_show_window(
        user, {title,
               rect_center_rect(sapp_screen_rect_scaled(user.state->dpi_scale),
                                {0, 0, 350, 76}),
               true});
    user.begin_generic(make_auto({AutoLayout::Column}, {0, 0}), {}, {});

    user.begin_generic(make_auto({AutoLayout::Row}), {}, {});

    UiBrush border =
        UiMakeBrush::make_gradient({0.0, 0.0, 0.0, 0.5}, {0.0, 0.0, 0.0, 0.0});

    switch (type)
    {
    default:
    case MsgBoxType::Info:
        user.begin_generic(make_element({AutoLayout::Column}, {40, 40}, false,
                                        false, {0.5, 0.5}, 1),
                           UiMakeBrush::make_gradient({0.3, 0.3, 1.0, 1.0},
                                                      {0, 0.2, 1.0, 0.4}),
                           border);
        label(user, "?", {2, 2}, UiMakeBrush::make_solid({1, 1, 1, 1.0}));
        break;
    case MsgBoxType::Warning:
        user.begin_generic(make_element({AutoLayout::Column}, {40, 40}, false,
                                        false, {0.5, 0.5}, 1),
                           UiMakeBrush::make_gradient({1.0, 0.8, 0.3, 1.0},
                                                      {1.0, 0.7, 0.0, 0.4}),
                           border);
        label(user, ":/", {2, 2}, UiMakeBrush::make_solid({1, 1, 1, 1.0}));
        break;
    case MsgBoxType::Error:
        user.begin_generic(make_element({AutoLayout::Column}, {40, 40}, false,
                                        false, {0.5, 0.5}, 1),
                           UiMakeBrush::make_gradient({1.0, 0.3, 0.3, 1.0},
                                                      {1.0, 0.0, 0.0, 0.4}),
                           border);
        label(user, "!", {2, 2}, UiMakeBrush::make_solid({1, 1, 1, 1.0}));
        break;
    }
    user.end_generic();

    label(user, text);

    user.end_generic();
    user.begin_generic(make_auto({AutoLayout::Row}), {}, {});

    int result = -1;
    for (int i = 0; buttons[i]; i++)
    {
        if (button(user, buttons[i]))
        {
            result = i;
        }
    }

    user.end_generic();
    user.end_generic();

    end_show_window(user);

    return result;
}
