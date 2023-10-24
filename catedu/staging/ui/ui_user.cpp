#include "ui_user.hpp"
#include "catedu/ui/rendering/colors.hpp"
#include "catedu/ui/rendering/make_brush.hpp"

const Vector4 theme[] = { { 0.6, 0.6, 0.6, 1.0 },
                          { 0.8, 0.8, 0.8, 1.0 },
                          { 0.0, 0.0, 0.0, 1.0 },
                          { 0.0, 0.0, 0.0, 1.0 } };

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
                   float scale = 1

)
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
    this->pass = UiRenderingPass::begin(&state->core);
}

void
UiUser::end_pass()
{
    this->pass.end();
}

bool
UiUser::put_button(const char* text)
{
    Vector2 size = { 300, 100 };
    Rect rect = { { 0, y }, size };

    draw_rectangle_gradient(pass, rect, theme[3], theme[3]);
    draw_rectangle_gradient(
      pass, rect_shrink(rect, { 1, 1 }), theme[0], theme[1]);
    draw_centered_text(text, pass, state->font, rect, theme[2], 2);
    y += size.y + 5;

    return false;
}

UiState
UiState::init(const char* font_path)
{
    UiState state = {};
    state.core = UiRenderingCore::init();
    state.font = UiFontRenderer::init(&state.core, { font_path, 16 });
    return state;
}
