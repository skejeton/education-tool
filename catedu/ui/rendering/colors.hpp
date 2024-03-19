#pragma once
#include "catedu/core/math/math.hpp"

#define UI_COLOR_BLACK (Vector4{0.0, 0.0, 0.0, 1.0})
#define UI_COLOR_WHITE (Vector4{1.0, 1.0, 1.0, 1.0})
#define UI_COLOR_RED (Vector4{1.0, 0.0, 0.0, 1.0})
#define UI_COLOR_GREEN (Vector4{0.0, 1.0, 0.0, 1.0})
#define UI_COLOR_BLUE (Vector4{0.0, 0.0, 1.0, 1.0})
#define UI_COLOR_YELLOW (Vector4{1.0, 1.0, 0.0, 1.0})
#define UI_COLOR_CYAN (Vector4{0.0, 1.0, 1.0, 1.0})
#define UI_COLOR_MAGENTA (Vector4{1.0, 0.0, 1.0, 1.0})

static inline Vector4 into_transparent(Vector4 color, float alpha = 0.0)
{
    return {color.x, color.y, color.z, alpha};
}

static inline Vector4 color_bluish(float mul = 1.0f, float alpha = 1.0f)
{
    return Vector4{0.93f * mul, 0.97f * mul, 1.0f * mul, alpha};
}

static inline Vector4 to_color(uint32_t color)
{
    return {
        (float)((color >> 24) & 0xFF) / 255.0f,
        (float)((color >> 16) & 0xFF) / 255.0f,
        (float)((color >> 8) & 0xFF) / 255.0f,
        (float)((color >> 0) & 0xFF) / 255.0f,
    };
}
