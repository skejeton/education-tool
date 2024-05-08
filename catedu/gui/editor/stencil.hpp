#pragma once
#include "catedu/core/math/math.hpp"
#include <functional>

enum class StencilType
{
    Freeform,
    Rectangle,
    Line,
    LineRectangle,
    Ellipse
};

using StencilEditMap = std::function<void(int x, int y, float angle)>;

struct StencilEdit
{
    StencilType type;
    Vector2i start;
    Vector2i end;

    void map(StencilEditMap f);
};
