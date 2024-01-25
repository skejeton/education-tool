#include "user.hpp"
#include "catedu/ui/layout/dpi_scale.hpp"
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
    this->state->element_storage.begin_cycle();
}

void render_out(UiUser &user)
{
    auto alloc = BumpAllocator::init();
    AutoLayoutResult *result;
    user.layout.process(alloc, result);
    dpi_rescale_autolayout_result(result, user.state->dpi_scale);

    while (result)
    {
        UiGenericStyles *styles = user.styles.get(result->userdata);
        if (styles)
        {
            bool hovered = false;
            int order = -1;

            if (styles->persistent != NULL_ID)
            {
                UiPersistentElement *pe =
                    user.state->element_storage.elements.get(
                        styles->persistent);
                order = pe->order;
                pe->border_box = result->border_box;
                if (user.state->interaction_table.hovered == styles->persistent)
                {
                    hovered = true;
                }
            }
            if (styles->text)
            {
                user.state->font.render_text_utf8(
                    &user.pass, result->base_box.pos, styles->text,
                    styles->brush, styles->text_scale * user.state->dpi_scale);
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
            if (hovered)
            {
                draw_rectangle_gradient(user.pass, result->border_box,
                                        {0.0, 0.3, 0.0, 0.2},
                                        {0.0, 0.3, 0.0, 0.2});
            }
            if (order >= 0)
            {
                user.state->font.render_text_utf8(
                    &user.pass, result->margin_box.pos - Vector2{10, 10},
                    stdstrfmt("%d", order).c_str(),
                    UiMakeBrush::make_solid({1.0, 0, 0, 1.0}),
                    Vector2{1.0f, 1.0f} * user.state->dpi_scale);
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
    UiInteractionStatePass interaction = {};
    interaction.interaction_table = &this->state->interaction_table;
    interaction.element_storage = &this->state->element_storage;
    interaction.mouse_pos = this->state->input.mouse_pos;
    interaction.interaction = this->state->input.mouse_pressed;
    interaction.exit_interaction = this->state->input.escape;
    interaction.switch_interaction = this->state->input.tab;
    interaction.process();

    render_out(*this);

    state->input.mouse_pressed = false;
    this->state->input.inputchars[0] = 0;
    this->state->input.escape = false;
    this->state->input.tab = false;
    this->state->input.inputchars_count = 0;
    this->state->element_storage.end_cycle();
    this->pass.end();
    this->styles.deinit();
    this->bump.deinit();
}

void UiUser::input(const char *id, char *out, int max)
{
    this->state->element_storage.push(id, {true});
    UiPersistentElement *pe = this->state->element_storage.value();

    bool focused = this->state->interaction_table.focused ==
                   this->state->element_storage.id();
    if (focused)
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

    AutoLayoutElement el = {};
    el.layout = {AutoLayout::Row};
    el.width = {AutoLayoutDimension::Auto};
    el.height = {AutoLayoutDimension::Auto};
    el.padding = {10, 10, 10, 10};
    el.margin = {5, 5, 5, 5};
    el.border = {1, 1, 1, 1};

    this->begin_generic(el, UiMakeBrush::make_solid(theme[0]),
                        focused ? UiMakeBrush::make_solid(theme[3])
                                : UiMakeBrush::make_solid(theme[1]),
                        this->state->element_storage.id());

    char *t = (char *)malloc(max + 3);
    memcpy(t, out, strlen(out));
    t[strlen(out)] = '|';
    t[strlen(out) + 1] = 0;

    this->label(t, {1.2, 1.2}, UiMakeBrush::make_solid(theme[2]));
    free(t);
    this->end_generic();

    this->state->element_storage.pop();
}

bool UiUser::button(const char *text, int offs)
{
    this->state->element_storage.push(text, {});
    UiPersistentElement *pe = this->state->element_storage.value();

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

    if (this->state->interaction_table.hovered ==
        this->state->element_storage.id())
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
                        UiMakeBrush::make_solid(theme[2]),
                        this->state->element_storage.id());

    this->label(text, {1, 1}, UiMakeBrush::make_solid(theme[3]));

    this->end_generic();
    this->state->element_storage.pop();

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
                           TableId persistent_id)
{
    UiGenericStyles styles = {brush, border, nullptr, {1, 1}, persistent_id};

    el.userdata = this->styles.allocate(styles);

    this->current_node = this->layout.add_element(this->current_node, el);
}

void UiUser::end_generic()
{
    auto node = this->layout.nodes.get(this->current_node.id);
    assert(node && "Begin generic was not called");

    this->current_node = node->parent;
}

UiState UiState::init(const char *font_path, float dpi_scale)
{
    UiState state = {};
    state.dpi_scale = dpi_scale;
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
        if (event->key_code == SAPP_KEYCODE_ESCAPE)
        {
            this->input.escape = true;
        }
        if (event->key_code == SAPP_KEYCODE_TAB)
        {
            this->input.tab = true;
        }
    case SAPP_EVENTTYPE_CHAR:
        if (this->input.inputchars_count < 8)
        {
            this->input.inputchars[this->input.inputchars_count++] =
                event->char_code;
        }
    default:
        break;
    }
    return this->interaction_table.focused != NULL_ID;
}

void UiState::deinit()
{
    this->element_storage.deinit();
    this->font.deinit();
    this->core->deinit();
    free(this->core);
}

void UiElementStorage::begin_cycle()
{
    this->element_retainer.begin_cycle();
    this->order = 0;
}

void UiElementStorage::end_cycle()
{
    this->element_retainer.end_cycle();
}

void UiElementStorage::push(const char *id, UiPersistentElement value)
{
    if (this->element_retainer.has(id) == false)
    {
        printf("Allocating %s\n", id);
        this->element_retainer.push(id, this->elements.allocate(value));
    }
    else
    {
        // TODO: This is a hack to make sure that the element is not freed
        //       the empty id isn't used.
        this->element_retainer.push(id, {});
    }
    this->value()->order = this->order;
    this->order++;
}

void UiElementStorage::pop()
{
    this->element_retainer.pop();
}

UiPersistentElement *UiElementStorage::value()
{
    return &this->elements.get_assert(*this->element_retainer.value());
}

TableId UiElementStorage::id()
{
    return *this->element_retainer.value();
}

UiElementStorage UiElementStorage::init()
{
    return UiElementStorage();
}

void UiElementStorage::deinit()
{
    this->elements.deinit();
    this->element_retainer.deinit();
}

void UiInteractionStatePass::process()
{
    TableId hovered = NULL_ID;
    TableId focused = this->interaction_table->focused;
    TableId prev_focused = this->interaction_table->focused;

    if (this->exit_interaction || this->interaction || this->switch_interaction)
    {
        focused = NULL_ID;
    }

    int max_order = -1;

    for (auto [id, el] : iter(this->element_storage->elements))
    {
        if (this->switch_interaction && el.focusable)
        {
            if (prev_focused != NULL_ID)
            {
                UiPersistentElement &pe =
                    this->element_storage->elements.get_assert(prev_focused);
                if (el.order > pe.order)
                {
                    focused = id;
                    this->switch_interaction = false;
                }
            }
        }

        if (rect_vs_vector2(el.border_box, this->mouse_pos) &&
            el.order > max_order)
        {
            if (this->interaction && el.focusable)
            {
                focused = id;
            }

            max_order = el.order;
            hovered = id;
        }

        // TODO: This is a hack to avoid freeing elements in the table for now.
        //       This should be removed when the element storage is refactored.
        el.order = -1;
    }

    this->interaction_table->hovered = hovered;
    this->interaction_table->focused = focused;
}
