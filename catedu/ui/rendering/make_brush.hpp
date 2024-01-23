#pragma once
#include "catedu/core/math/math.hpp"
#include "core.hpp"

struct UiMakeBrush
{
    UiBrush brush;

    static UiMakeBrush make_plain_brush(UiBuffers shape);
    static UiMakeBrush make_image_brush(UiBuffers shape, UiRenderingCore *core,
                                        UiImageId image);
    static UiBrush make_solid(Vector4 color);
    static UiBrush make_gradient(Vector4 color_bottom, Vector4 color_top);

    UiMakeBrush &with_solid(Vector4 color);
    UiMakeBrush &with_gradient(Vector4 color_bottom, Vector4 color_top);
    UiMakeBrush &with_image_region(Rect region);
    UiBrush build();
};
