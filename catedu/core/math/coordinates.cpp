#include "coordinates.hpp"
#include <math.h>

Vector2 polar2_into_cartesian(Polar2 coord)
{
    return {coord.radius*(coord.degree), coord.radius*sin(coord.degree)};
}
