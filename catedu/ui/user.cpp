#include "user.hpp"
#include "catedu/ui/layout/dpi_scale.hpp"
#include "catedu/ui/rendering/make_brush.hpp"

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

void draw_brush(UiRenderingPass &pass, Rect rect, UiBrush brush)
{
    UiTransform transform = {};
    transform.scale = {1, 1};
    transform.base = rect;

    pass.push_transform(transform);

    pass.render_brush(brush);

    pass.pop_transform();
}

UiUser UiUser::init(UiState &state)
{
    return UiUser{&state};
}

void UiUser::begin_pass()
{
    this->layout = AutoLayoutProcess::init(this->current_node);
    this->bump = BumpAllocator::init();
    this->state->element_storage.begin_cycle();
    if (this->state->dpi_scale > 10.0)
    {
        this->state->dpi_scale = 10.0;
    }
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
                if (result->hidden)
                {
                    pe->border_box = {};
                }
                pe->border_box = result->border_box;
                if (user.state->interaction_table.hovered == styles->persistent)
                {
                    hovered = true;
                }
            }
            if (result->hidden)
            {
                result = result->next;
                continue;
            }
            if (styles->text)
            {
                if (styles->bold)
                {
                    user.state->font_bold.render_text_utf8(
                        &user.pass, result->base_box.pos, styles->text,
                        styles->brush,
                        styles->text_scale * user.state->dpi_scale);
                }
                else
                {
                    user.state->font.render_text_utf8(
                        &user.pass, result->base_box.pos, styles->text,
                        styles->brush,
                        styles->text_scale * user.state->dpi_scale);
                }
            }
            else
            {
                draw_brush(user.pass, result->border_box, styles->border);
                draw_brush(user.pass, result->base_box, styles->brush);
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
    interaction.interaction = this->state->input.k[INPUT_MB_LEFT].pressed;
    interaction.active_interaction = this->state->input.k[INPUT_MB_LEFT].held;
    interaction.exit_interaction =
        this->state->input.k[SAPP_KEYCODE_ESCAPE].pressed;
    interaction.switch_interaction =
        this->state->input.k[SAPP_KEYCODE_TAB].pressed;
    interaction.process();

    this->pass = UiRenderingPass::begin(state->core);
    render_out(*this);
    this->pass.end();

    this->state->input.update();
    this->state->element_storage.end_cycle();
    this->styles.deinit();
    this->bump.deinit();
}

void UiUser::push_id(int64_t id)
{
}

void UiUser::push_id(const char *id)
{
    this->state->element_storage.push(id, {});
}

void UiUser::pop_id()
{
    this->state->element_storage.pop();
}

bool UiUser::focused()
{
    return state->interaction_table.focused == state->element_storage.id();
}

bool UiUser::active()
{
    return state->interaction_table.active == state->element_storage.id();
}

bool UiUser::hovered()
{
    return state->interaction_table.hovered == state->element_storage.id();
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

UiState UiState::init(const char *font_path, const char *font_bold_path,
                      float dpi_scale)
{
    UiState state = {};
    state.dpi_scale = dpi_scale;
    state.core =
        (UiRenderingCore *)OOM_HANDLER(malloc(sizeof(UiRenderingCore)));
    *state.core = UiRenderingCore::init();
    state.font =
        UiFontRenderer::init(state.core, {font_path, 16}, dpi_scale * 2);
    state.font_bold =
        UiFontRenderer::init(state.core, {font_bold_path, 16}, dpi_scale * 2);
    return state;
}

bool UiState::feed_event(const sapp_event *event)
{
    this->input.pass_event(event);
    if (this->input.shortcut(INPUT_CTRL, SAPP_KEYCODE_MINUS))
    {
        this->dpi_scale /= 1.125;
    }
    if (this->input.shortcut(INPUT_CTRL, SAPP_KEYCODE_EQUAL))
    {
        this->dpi_scale *= 1.125;
    }

    return this->interaction_table.focused != NULL_ID ||
           this->interaction_table.hovered != NULL_ID;
}

void UiState::deinit()
{
    this->element_storage.deinit();
    this->font_bold.deinit();
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
        // TODO: This is a hack to make sure that the element isn't reallocated.
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
    return &this->elements.get_assert(this->element_retainer.value());
}

TableId UiElementStorage::id()
{
    return this->element_retainer.value();
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

    TableId active = NULL_ID;

    if (this->active_interaction)
    {
        if (this->interaction_table->active == NULL_ID)
        {
            active = this->interaction_table->hovered;
        }
        else
        {
            active = this->interaction_table->active;
        }
    }

    this->interaction_table->hovered = hovered;
    this->interaction_table->active = active;
    this->interaction_table->focused = focused;
}
