#ifndef HPP_CATEDU_CORE_MATH_POINT_INTERSECT
#define HPP_CATEDU_CORE_MATH_POINT_INTERSECT

#include "catedu/core/math/quintic_superellipse.hpp"
#include "catedu/math.hpp"

bool
math_point_intersect_rect(Vector2 point, Rect rect);
bool
math_point_intersect_ellipse(Vector2 point, Vector2 center, Vector2 radii);
bool
math_point_intersect_squircle(Vector2 point, Rect rect, float n);

#endif
