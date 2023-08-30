#include "quintic_superellipse.hpp"
#include <math.h>

Polar2
quintic_superellipse_generate(float n, float a, float b, float theta)
{
    float x = pow(fabs(cos(theta) / a), n);
    float y = pow(fabs(sin(theta) / b), n);
    float r = pow(x + y, -1 / n);

    return Polar2{ r, theta };
}

bool
quintic_superellipse_intersect_point(float n, float a, float b, Vector2 point)
{
    // This works by transforming the point into the first quadrant, then
    // checking if the point is inside the superellipse.
    float x = pow(fabs(point.x / a), n);
    float y = pow(fabs(point.y / b), n);
    float r = x + y;

    return r <= 1;
}
