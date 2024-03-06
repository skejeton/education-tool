#pragma once
#include "coordinates.hpp"

/**
 * @param n The power of the superellipse.
 * @param radius The radius of the superellipse.
 * @param theta The angle of the point on the superellipse.
 * @return The point on the superellipse.
 */
Polar2 superellipse_generate(float n, Vector2 radius, float theta);

/**
 * @param n The power of the superellipse.
 * @param radius The radius of the superellipse.
 * @param point The point to check.
 * @return Whether the point is inside the superellipse.
 */
bool superellipse_intersect_point(float n, Vector2 radius, Vector2 point);
