#pragma once
#include "catedu/core/math/math.hpp"
#include "catedu/rendering/color.hpp"
#include "core.hpp"

struct UiMakeBrush
{
    UiBrush brush;

    static UiMakeBrush make_plain_brush();
    static UiMakeBrush make_image_brush(UiRenderingCore *core, UiImageId image);

    static UiBrush make_solid(Color color);
    static UiBrush make_gradient(Color color_bottom, Color color_top);

    UiMakeBrush &squircle(float radius);
    UiMakeBrush &squircle(float radiusx, float radiusy);
    UiMakeBrush &with_solid(Color color);
    UiMakeBrush &with_gradient(Color color_bottom, Color color_top);
    UiMakeBrush &with_image_region(Rect region);
    UiBrush build();
};
