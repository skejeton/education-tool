#ifndef HPP_CATEDU_CORE_MATH_COORDINATES
#define HPP_CATEDU_CORE_MATH_COORDINATES

#include "catedu/math.hpp"

struct Polar2 {
    float radius, degree;
};

Vector2 polar2_into_cartesian(Polar2 coord);

#endif

