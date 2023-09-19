#include "coordinates.hpp"
#include <math.h>

Vector2
polar2_into_cartesian(Polar2 coord)
{
    return { coord.radius * cos(coord.theta), coord.radius * sin(coord.theta) };
}

Polar2
cartesian_into_polar2(Vector2 coord)
{
    return { sqrt(coord.x * coord.x + coord.y * coord.y),
             atan2(coord.y, coord.x) };
}

Vector3
polar3_into_cartesian(Polar3 coord)
{
    return { coord.radius * cos(coord.theta) * sin(coord.phi),
             coord.radius * sin(coord.theta) * sin(coord.phi),
             coord.radius * cos(coord.phi) };
}

Polar3
cartesian_into_polar3(Vector3 coord)
{
    return { sqrt(coord.x * coord.x + coord.y * coord.y + coord.z * coord.z),
             atan2(coord.y, coord.x),
             acos(coord.z / sqrt(coord.x * coord.x + coord.y * coord.y +
                                 coord.z * coord.z)) };
}
