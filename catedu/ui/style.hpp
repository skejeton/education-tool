#pragma once

#include "catedu/ui/rendering/core.hpp"
struct UiStyle
{
    UiBrush nx_background;
    UiBrush nx_border;
    UiBrush nx_color;
    float nx_border_size;
    float nx_margin;
    float nx_padding;

    UiStyle &background(UiBrush brush);

    UiStyle &border(UiBrush brush);

    UiStyle &color(UiBrush brush);

    UiStyle &border_size(float size);

    UiStyle &margin(float size);

    UiStyle &padding(float size);
};

struct UiStyleHandler
{
};
