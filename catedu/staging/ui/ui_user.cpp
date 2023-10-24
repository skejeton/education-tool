#include "ui_user.hpp"
#include "catedu/ui/rendering/colors.hpp"
#include "catedu/ui/rendering/make_brush.hpp"

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
    UiTransform transform = {};
    transform.scale = { 1, 1 };
    transform.base = { 0, 0, 200, 200 };

    this->pass.push_transform(transform);

    this->pass.render_brush(
      UiMakeBrush::make_plain_brush(UiBuffers::Rectangle)
        .with_gradient({ 0.6, 0.6, 0.6, 1.0 }, { 0.8, 0.8, 0.8, 1.0 })
        .build());

    this->pass.pop_transform();

    return true;
}

UiState
UiState::init(const char* font_path)
{
    UiState state = {};
    state.core = UiRenderingCore::init();
    state.font = UiFontRenderer::init(&state.core, { font_path, 16 });
    return state;
}
