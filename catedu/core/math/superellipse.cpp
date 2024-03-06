#include "superellipse.hpp"
#include <math.h>

Polar2 superellipse_generate(float n, Vector2 radius, float theta)
{
    float x = pow(fabs(cos(theta) / radius.x), n);
    float y = pow(fabs(sin(theta) / radius.y), n);
    float r = pow(x + y, -1 / n);

    return Polar2{r, theta};
}

bool superellipse_intersect_point(float n, Vector2 radius, Vector2 point)
{
    // This works by transforming the point into the first quadrant, then
    // checking if the point is inside the superellipse.
    float x = pow(fabs(point.x / radius.x), n);
    float y = pow(fabs(point.y / radius.y), n);
    float r = x + y;

    return r <= 1;
}
