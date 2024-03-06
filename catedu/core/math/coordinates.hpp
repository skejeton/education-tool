#pragma once
#include "math.hpp"

struct Polar2
{
    float radius, theta;
};

struct Polar3
{
    float radius, theta, phi;
};

Vector2 polar2_into_cartesian(Polar2 coord);

Polar2 cartesian_into_polar2(Vector2 coord);

Vector3 polar3_into_cartesian(Polar3 coord);

Polar3 cartesian_into_polar3(Vector3 coord);
