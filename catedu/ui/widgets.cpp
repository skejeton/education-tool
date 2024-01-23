#include "widgets.hpp"

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
        user.label(info.title, {1, 1});
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
