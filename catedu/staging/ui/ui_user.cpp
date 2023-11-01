#include "ui_user.hpp"
#include "catedu/ui/rendering/colors.hpp"
#include "catedu/ui/rendering/make_brush.hpp"

const Vector4 theme[] = { { 0.8, 0.8, 0.8, 1.0 }, { 0.6, 0.6, 0.6, 1.0 },
                          { 0.0, 0.0, 0.0, 1.0 }, { 0.0, 0.0, 0.0, 1.0 },
                          { 0.7, 0.7, 0.7, 1.0 }, { 0.5, 0.5, 0.5, 1.0 } };

void
draw_rectangle_gradient(UiRenderingPass& pass,
                        Rect rect,
                        Vector4 color_top,
                        Vector4 color_bottom)
{
    UiTransform transform = {};
    transform.scale = { 1, 1 };
    transform.base = rect;

    pass.push_transform(transform);

    pass.render_brush(UiMakeBrush::make_gradient(color_bottom, color_top));

    pass.pop_transform();
}

void
draw_centered_text(const char* text,
                   UiRenderingPass& pass,
                   UiFontRenderer& font,
                   Rect rect,
                   Vector4 color,
                   float scale = 1)
{
    Rect bounds = font.bounds_text_utf8(rect.pos, text, { scale, scale });
    Rect text_rect = rect_center_rect(rect, bounds);
    Vector2 text_pos = text_rect.pos;

    font.render_text_utf8(
      &pass, text_pos, text, UiMakeBrush::make_solid(color), { scale, scale });
}

UiUser
UiUser::init(UiState& state)
{
    return UiUser{ &state };
}

void
UiUser::begin_pass()
{
    this->layout = AutoLayoutProcess::init(this->current_node);
    this->pass = UiRenderingPass::begin(&state->core);
}

void
render_out(UiUser& user)
{
    auto alloc = BumpAllocator::init();
    AutoLayoutResult* result;
    user.layout.process(alloc, result);

    while (result) {
        draw_rectangle_gradient(user.pass, result->rect, theme[2], theme[2]);
        draw_rectangle_gradient(user.pass, rect_shrink(result->rect, {1, 1}), theme[1], theme[1]);
        result = result->next;
    }

    if (user.current_node.id != user.layout.root.id) {
        assert(false && "Unfinished begin_generic calls");
    }

    user.layout.deinit();
    alloc.deinit();
}

void
UiUser::end_pass()
{
    render_out(*this);

    state->input.mouse_pressed = false;
    this->pass.end();
}

bool
UiUser::button(const char* text)
{
    Vector2 size = { 300, 100 };
    Rect rect = { { 10, 10 }, size };
    Vector4 color1 = theme[0];
    Vector4 color2 = theme[1];

    bool pressed = false;

    if (rect_vs_vector2(rect, state->input.mouse_pos)) {
        color1 = theme[4];
        color2 = theme[5];
        if (state->input.mouse_down) {
            pressed = state->input.mouse_pressed;
            std::swap(color1, color2);
        }
    }
    draw_rectangle_gradient(pass, rect, theme[3], theme[3]);
    draw_rectangle_gradient(pass, rect_shrink(rect, { 1, 1 }), color1, color2);
    draw_centered_text(text, pass, state->font, rect, theme[2], 2);

    return pressed;
}

void
UiUser::label(const char* text)
{
    Rect r = state->font.bounds_text_utf8({ 10, 10 }, text, { 2, 2 });
    state->font.render_text_utf8(
      &pass, r.pos, text, UiMakeBrush::make_solid(theme[2]), { 2, 2 });
}

void
UiUser::begin_generic(Vector2 size,
                      UiBrush brush,
                      float border_width,
                      UiBrush border)
{
    AutoLayoutElement el = {};
    el.base_size = size;
    el.border = { border_width, border_width, border_width, border_width };
    el.layout.type = AutoLayout::Column;

    this->current_node = this->layout.add_element(this->current_node, el);
}

void
UiUser::end_generic()
{
    auto node = this->layout.nodes.get(this->current_node.id);
    assert(node && "Begin generic was not called");

    this->current_node = node->parent;
}

UiState
UiState::init(const char* font_path)
{
    UiState state = {};
    state.core = UiRenderingCore::init();
    state.font = UiFontRenderer::init(&state.core, { font_path, 16 });
    return state;
}

void
UiState::feed_event(const sapp_event* event)
{
    switch (event->type) {
        case SAPP_EVENTTYPE_MOUSE_MOVE:
            this->input.mouse_pos = { event->mouse_x, event->mouse_y };
            this->input.mouse_delta = { event->mouse_dx, event->mouse_dy };
            break;
        case SAPP_EVENTTYPE_MOUSE_DOWN:
        case SAPP_EVENTTYPE_MOUSE_UP:
            this->input.mouse_down =
              (event->mouse_button == SAPP_MOUSEBUTTON_LEFT) &&
              event->type == SAPP_EVENTTYPE_MOUSE_DOWN;
            this->input.mouse_pressed = this->input.mouse_down;
            break;
    }
}
