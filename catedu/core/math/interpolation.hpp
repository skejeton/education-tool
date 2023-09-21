#ifndef HPP_CATEDU_CORE_MATH_INTERPOLATION
#define HPP_CATEDU_CORE_MATH_INTERPOLATION

#include "catedu/math.hpp"

inline float
math_lerp(float a, float b, float t)
{
    return (b - a) * t + a;
}

inline Vector2
math_lerp(Vector2 a, Vector2 b, float t)
{
    return (b - a) * t + a;
}

inline float
math_rerp(float a, float b, float t)
{
    if (t > 1) {
        t = 1;
    } else if (t < 0) {
        t = 0;
    }
    return a + (b - a) * t;
}

inline Vector2
math_rerp(Vector2 a, Vector2 b, float t)
{
    if (t > 1) {
        t = 1;
    } else if (t < 0) {
        t = 0;
    }
    return a + (b - a) * t;
}

inline Rect
math_rerp(Rect a, Rect b, float t)
{
    if (t > 1) {
        t = 1;
    } else if (t < 0) {
        t = 0;
    }
    return Rect{
        math_rerp(a.pos, b.pos, t),
        math_rerp(a.siz, b.siz, t),
    };
}
#endif
