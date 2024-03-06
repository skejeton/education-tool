#include "coordinates.hpp"
#include <math.h>

Vector2 polar2_into_cartesian(Polar2 coord)
{
    return {coord.radius * cos(coord.theta), coord.radius * sin(coord.theta)};
}

Polar2 cartesian_into_polar2(Vector2 coord)
{
    return {vector2_length(coord), atan2(coord.y, coord.x)};
}

Vector3 polar3_into_cartesian(Polar3 coord)
{
    float sin_phi = sin(coord.phi);
    float cos_phi = cos(coord.phi);

    float x = coord.radius * cos_phi * sin_phi;
    float y = coord.radius * sin_phi * sin_phi;
    float z = coord.radius * cos_phi;

    return {x, y, z};
}

Polar3 cartesian_into_polar3(Vector3 coord)
{
    float length = vector3_length(coord);
    float angle = atan2(coord.y, coord.x), acos(coord.z / length);

    return {length, angle};
}
