#include "ui_user.hpp"
#include "catedu/ui/rendering/make_brush.hpp"

const Vector4 theme[] = {{0.8, 0.8, 0.8, 1.0}, {0.6, 0.6, 0.6, 1.0},
                         {0.0, 0.0, 0.0, 1.0}, {0.0, 0.0, 0.0, 1.0},
                         {0.7, 0.7, 0.7, 1.0}, {0.5, 0.5, 0.5, 1.0}};

void draw_rectangle_gradient(UiRenderingPass &pass, Rect rect,
                             Vector4 color_top, Vector4 color_bottom)
{
    UiTransform transform = {};
    transform.scale = {1, 1};
    transform.base = rect;

    pass.push_transform(transform);

    pass.render_brush(UiMakeBrush::make_gradient(color_bottom, color_top));

    pass.pop_transform();
}

UiUser UiUser::init(UiState &state)
{
    return UiUser{&state};
}

void UiUser::begin_pass()
{
    this->layout = AutoLayoutProcess::init(this->current_node);
    this->pass = UiRenderingPass::begin(state->core);
    this->bump = BumpAllocator::init();
}

void render_out(UiUser &user)
{
    auto alloc = BumpAllocator::init();
    AutoLayoutResult *result;
    user.layout.process(alloc, result);

    while (result)
    {
        UiGenericStyles *styles = user.styles.get(result->userdata);
        if (styles)
        {
            if (styles->persistent)
            {
                styles->persistent->border_box = result->border_box;
            }
            if (styles->text)
            {
                user.state->font.render_text_utf8(
                    &user.pass, result->base_box.pos, styles->text,
                    styles->brush, styles->text_scale);
            }
            else
            {
                draw_rectangle_gradient(user.pass, result->border_box,
                                        styles->border.color_bottom,
                                        styles->border.color_top);
                draw_rectangle_gradient(user.pass, result->base_box,
                                        styles->brush.color_bottom,
                                        styles->brush.color_top);
            }
        }
        result = result->next;
    }

    assert(user.current_node.id == user.layout.root.id &&
           "Unfinished begin_generic calls");

    user.layout.deinit();
    alloc.deinit();
}

void UiUser::end_pass()
{
    render_out(*this);

    state->input.mouse_pressed = false;
    this->state->input.inputchars[0] = 0;
    this->state->input.inputchars_count = 0;
    this->pass.end();
    this->styles.deinit();
    this->bump.deinit();
}

void UiUser::input(char *id, char *out, int max)
{
    this->state->elements.push(id, {});
    UiPersistentElement *pe = this->state->elements.value();

    bool coll = rect_vs_vector2(pe->border_box, this->state->input.mouse_pos);
    if (this->state->textfieldfocus && coll)
    {
        for (int i = 0; i < this->state->input.inputchars_count; i++)
        {
            if (this->state->input.inputchars[i] == '\b')
            {
                if (strlen(out) > 0)
                    out[strlen(out) - 1] = 0;
            }
            else if (this->state->input.inputchars[i])
            {
                if (strlen(out) < max - 1)
                {
                    out[strlen(out)] = this->state->input.inputchars[i];
                }
            }
        }
    }
    this->state->textfieldfocus = this->state->textfieldfocus || coll;

    AutoLayoutElement el = {};
    el.layout = {AutoLayout::Row};
    el.width = {AutoLayoutDimension::Auto};
    el.height = {AutoLayoutDimension::Auto};
    el.padding = {10, 10, 10, 10};
    el.margin = {5, 5, 5, 5};
    el.border = {1, 1, 1, 1};

    this->begin_generic(el, UiMakeBrush::make_solid(theme[0]),
                        coll ? UiMakeBrush::make_solid(theme[3])
                             : UiMakeBrush::make_solid(theme[1]),
                        pe);

    char *t = (char *)malloc(max + 3);
    memcpy(t, out, strlen(out));
    t[strlen(out)] = '|';
    t[strlen(out) + 1] = 0;

    this->label(t, {2, 2}, UiMakeBrush::make_solid(theme[2]));
    free(t);
    this->end_generic();

    this->state->elements.pop();
}

bool UiUser::button(const char *text)
{
    this->state->elements.push(text, {});
    UiPersistentElement *pe = this->state->elements.value();

    AutoLayoutElement el = {};
    el.layout = {AutoLayout::Row};
    el.width = {AutoLayoutDimension::Auto};
    el.height = {AutoLayoutDimension::Auto};
    el.padding = {10, 10, 10, 10};
    el.margin = {5, 5, 5, 5};
    el.border = {1, 1, 1, 1};

    Vector4 color_top = theme[1];
    Vector4 color_bottom = theme[0];

    bool pressed = false;

    if (rect_vs_vector2(pe->border_box, this->state->input.mouse_pos))
    {
        color_top = theme[5];
        color_bottom = theme[4];
        if (this->state->input.mouse_down)
        {
            std::swap(color_top, color_bottom);
        }
        pressed = this->state->input.mouse_pressed;
    }

    this->begin_generic(el, UiMakeBrush::make_gradient(color_bottom, color_top),
                        UiMakeBrush::make_solid(theme[2]), pe);

    this->label(text, {2, 2}, UiMakeBrush::make_solid(theme[3]));

    this->end_generic();
    this->state->elements.pop();

    return pressed;
}

void UiUser::label(const char *text, Vector2 scale, UiBrush style)
{
    Vector2 size = this->state->font.bounds_text_utf8({0, 0}, text, scale).siz;

    void *ptr = this->bump.alloc(strlen(text) + 1);
    strcpy((char *)ptr, text);

    AutoLayoutElement el = {};
    el.layout = {AutoLayout::Row};
    el.width = {AutoLayoutDimension::Pixel, size.x};
    el.height = {AutoLayoutDimension::Pixel, size.y};
    el.userdata =
        this->styles.allocate(UiGenericStyles{style, {}, (char *)ptr, scale});

    this->layout.add_element(this->current_node, el);
}

void UiUser::begin_generic(AutoLayoutElement el, UiBrush brush, UiBrush border,
                           UiPersistentElement *persistent)
{
    UiGenericStyles styles = {brush, border, nullptr, {1, 1}, persistent};

    el.userdata = this->styles.allocate(styles);

    this->current_node = this->layout.add_element(this->current_node, el);
}

void UiUser::end_generic()
{
    auto node = this->layout.nodes.get(this->current_node.id);
    assert(node && "Begin generic was not called");

    this->current_node = node->parent;
}

UiState UiState::init(const char *font_path)
{
    UiState state = {};
    state.core = (UiRenderingCore *)malloc(sizeof(UiRenderingCore));
    *state.core = UiRenderingCore::init();
    state.font = UiFontRenderer::init(state.core, {font_path, 16});
    return state;
}

bool UiState::feed_event(const sapp_event *event)
{
    switch (event->type)
    {
    case SAPP_EVENTTYPE_MOUSE_MOVE:
        this->input.mouse_pos = {event->mouse_x, event->mouse_y};
        this->input.mouse_delta = {event->mouse_dx, event->mouse_dy};
        this->textfieldfocus = false;
        break;
    case SAPP_EVENTTYPE_MOUSE_DOWN:
    case SAPP_EVENTTYPE_MOUSE_UP:
        this->input.mouse_down =
            (event->mouse_button == SAPP_MOUSEBUTTON_LEFT) &&
            event->type == SAPP_EVENTTYPE_MOUSE_DOWN;
        this->input.mouse_pressed = this->input.mouse_down;
        break;
    case SAPP_EVENTTYPE_KEY_DOWN:
        if (event->key_code == SAPP_KEYCODE_BACKSPACE)
        {
            if (this->input.inputchars_count < 8)
            {
                this->input.inputchars[this->input.inputchars_count++] = '\b';
            }
        }
        if (event->key_code == SAPP_KEYCODE_ENTER)
        {
            if (this->input.inputchars_count < 8)
            {
                this->input.inputchars[this->input.inputchars_count++] = '\n';
            }
        }
        return this->textfieldfocus;
    case SAPP_EVENTTYPE_CHAR:
        if (this->input.inputchars_count < 8)
        {
            this->input.inputchars[this->input.inputchars_count++] =
                event->char_code;
        }
        return this->textfieldfocus;
    }
    return false;
}

void UiState::deinit()
{
    this->elements.deinit();
    this->font.deinit();
    this->core->deinit();
    free(this->core);
}
