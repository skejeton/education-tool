#ifndef HPP_CATEDU_UI_RENDERING_COLORS_HPP
#define HPP_CATEDU_UI_RENDERING_COLORS_HPP

#include "catedu/math.hpp"

#define UI_COLOR_BLACK (Vector4{ 0.0, 0.0, 0.0, 1.0 })
#define UI_COLOR_WHITE (Vector4{ 1.0, 1.0, 1.0, 1.0 })
#define UI_COLOR_RED (Vector4{ 1.0, 0.0, 0.0, 1.0 })
#define UI_COLOR_GREEN (Vector4{ 0.0, 1.0, 0.0, 1.0 })
#define UI_COLOR_BLUE (Vector4{ 0.0, 0.0, 1.0, 1.0 })
#define UI_COLOR_YELLOW (Vector4{ 1.0, 1.0, 0.0, 1.0 })
#define UI_COLOR_CYAN (Vector4{ 0.0, 1.0, 1.0, 1.0 })
#define UI_COLOR_MAGENTA (Vector4{ 1.0, 0.0, 1.0, 1.0 })

Vector4
into_transparent(Vector4 color, float alpha = 0.0)
{
    return { color.x, color.y, color.z, alpha };
}

static Vector4
color_bluish(float mul = 1.0f, float alpha = 1.0f)
{
    return Vector4{ 0.93f * mul, 0.97f * mul, 1.0f * mul, alpha };
}

#endif
