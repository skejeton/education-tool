#include "ux.hpp"
#include "catedu/ui/rendering/make_brush.hpp"
#include "catedu/ui/widgets.hpp"

UX UX::begin(UiState &state)
{
    UX ux = {};
    ux.state = &state;
    ux.pass = UiPass::init(state);
    ux.pass.begin_pass();

    return ux;
}

void UX::end()
{
    pass.end_pass();
}

// EVENTS:
bool UX::clicked()
{
    return pass.state->input.k[INPUT_MB_LEFT].held && pass.active();
}

// MARK: Style
#pragma region style
UX &UX::background_color(Color color)
{
    nx_background = UiMakeBrush::make_solid(color);
    return *this;
}

UX &UX::border_size(float px)
{
    nx_border_size = px;
    return *this;
}

UX &UX::border_color(Color color)
{
    nx_border = UiMakeBrush::make_solid(color);
    return *this;
}
#pragma endregion

// MARK: Layout
#pragma region layout
UX &UX::row(std::function<void()> cb)
{
    // TODO: Add check if element is hidden, and don't render it.

    AutoLayoutElement el = {};
    el.border = {nx_border_size, nx_border_size, nx_border_size,
                 nx_border_size};
    el.layout.type = AutoLayout::row;
    pass.begin_generic(el, nx_background, nx_border);
    nx_background = {};
    nx_border = {};
    nx_border_size = {};
    cb();
    pass.end_generic();
    return *this;
}

UX &UX::column(std::function<void()> cb)
{
    // TODO: Add check if element is hidden, and don't render it.

    AutoLayoutElement el = {};
    el.border = {nx_border_size, nx_border_size, nx_border_size,
                 nx_border_size};
    el.layout.type = AutoLayout::column;
    pass.begin_generic(el, nx_background, nx_border);
    nx_background = {};
    nx_border = {};
    nx_border_size = {};
    cb();
    pass.end_generic();
    return *this;
}
#pragma endregion

// MARK: Widgets
#pragma region widgets
bool UX::button(const char *text)
{
    // TODO: Remove the widgets from external access once everything is using
    // this UX structure.
    return ::button(pass, text);
}

UX &UX::label(const char *text)
{
    ::label(pass, text, {1, 1}, UiMakeBrush::make_solid(0xFFFFFFFF));

    return *this;
}

UX &UX::heading(const char *text)
{
    pass.bold = true;
    ::label(pass, text, {1.5, 1.5}, UiMakeBrush::make_solid(0xFFFFFFFF));
    pass.bold = false;

    return *this;
}
#pragma endregion
