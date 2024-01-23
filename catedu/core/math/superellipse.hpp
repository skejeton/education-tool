#pragma once
#include "coordinates.hpp"

/**
 * @brief Generates a point on a superellipse.
 *
 * @param n The power of the superellipse.
 * @param a The x-radius of the superellipse.
 * @param b The y-radius of the superellipse.
 * @param theta The angle of the point on the superellipse.
 * @return The point on the superellipse.
 */
Polar2 superellipse_generate(float n, float a, float b, float theta);

/**
 * @brief Checks if a point is inside a superellipse.
 *
 * @param n The power of the superellipse.
 * @param a The x-radius of the superellipse.
 * @param b The y-radius of the superellipse.
 * @param point The point to check.
 * @return Whether the point is inside the superellipse.
 */
bool superellipse_intersect_point(float n, float a, float b, Vector2 point);
