#include "point_intersect.hpp"

bool math_point_intersect_rect(Vector2 point, Rect rect)
{
    return point.x >= rect.pos.x && point.x <= rect.pos.x + rect.siz.x &&
           point.y >= rect.pos.y && point.y <= rect.pos.y + rect.siz.y;
}

bool math_point_intersect_ellipse(Vector2 point, Vector2 center, Vector2 radii)
{
    return superellipse_intersect_point(2, radii, point - center);
}

bool math_point_intersect_squircle(Vector2 point, Rect rect, float n)
{
    return superellipse_intersect_point(n, rect.siz / 2.0f,
                                        point - rect.pos - rect.siz / 2.0f);
}
