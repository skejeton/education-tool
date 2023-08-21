#include "quintic_superellipse.hpp"
#include <math.h>

Polar2 quintic_superellipse_generate(float n, float a, float b, float theta)
{
    float x = pow(fabs(cos(theta)/a), n);
    float y = pow(fabs(sin(theta)/b), n);
    float r = pow(x + y, -1/n);

    return Polar2{r, theta};
}
