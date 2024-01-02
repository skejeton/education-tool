#pragma once
#include "catedu/math.hpp"

/**
 * @brief A polar coordinate in 2D space.
 */
struct Polar2
{
    float radius, theta;
};

/**
 * @brief A polar coordinate in 3D space.
 */
struct Polar3
{
    float radius, theta, phi;
};

/**
 * @brief Converts a polar coordinate into a cartesian coordinate.
 */
Vector2 polar2_into_cartesian(Polar2 coord);

/**
 * @brief Converts a cartesian coordinate into a polar coordinate.
 */
Polar2 cartesian_into_polar2(Vector2 coord);

/**
 * @brief Converts a polar coordinate into a cartesian coordinate.
 */
Vector3 polar3_into_cartesian(Polar3 coord);

/**
 * @brief Converts a cartesian coordinate into a polar coordinate.
 */
Polar3 cartesian_into_polar3(Vector3 coord);
