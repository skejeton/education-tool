#pragma once
#include "catedu/core/math/superellipse.hpp"
#include "catedu/math.hpp"

bool math_point_intersect_rect(Vector2 point, Rect rect);
bool math_point_intersect_ellipse(Vector2 point, Vector2 center, Vector2 radii);
bool math_point_intersect_squircle(Vector2 point, Rect rect, float n);
