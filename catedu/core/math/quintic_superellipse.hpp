#ifndef HPP_CATEDU_CORE_MATH_QUINTIC_SUPERELLIPSE
#define HPP_CATEDU_CORE_MATH_QUINTIC_SUPERELLIPSE

#include "coordinates.hpp"

/**
 * @brief Generates a point on a quintic superellipse.
 *
 * @param n The power of the superellipse.
 * @param a The x-radius of the superellipse.
 * @param b The y-radius of the superellipse.
 * @param theta The angle of the point on the superellipse.
 * @return The point on the superellipse.
 */
Polar2
quintic_superellipse_generate(float n, float a, float b, float theta);

/**
 * @brief Checks if a point is inside a quintic superellipse.
 *
 * @param n The power of the superellipse.
 * @param a The x-radius of the superellipse.
 * @param b The y-radius of the superellipse.
 * @param point The point to check.
 * @return Whether the point is inside the superellipse.
 */
bool
quintic_superellipse_intersect_point(float n, float a, float b, Vector2 point);

#endif
