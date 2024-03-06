#pragma once
#include "math.hpp"

/// @brief Linearly interpolates between two floats.
inline float math_lerp(float a, float b, float t)
{
    return (b - a) * t + a;
}

/// @brief Linearly interpolates between two vectors.
inline Vector2 math_lerp(Vector2 a, Vector2 b, float t)
{
    return (b - a) * t + a;
}

/// @brief Exponentially interpolates between two floats.
inline float math_rerp(float a, float b, float t)
{
    if (t > 1)
    {
        t = 1;
    }
    else if (t < 0)
    {
        t = 0;
    }
    return a + (b - a) * t;
}

/// @brief Exponentially interpolates between two vectors.
inline Vector2 math_rerp(Vector2 a, Vector2 b, float t)
{
    if (t > 1)
    {
        t = 1;
    }
    else if (t < 0)
    {
        t = 0;
    }
    return a + (b - a) * t;
}

/// @brief Exponentially interpolates between two rectangles.
inline Rect math_rerp(Rect a, Rect b, float t)
{
    if (t > 1)
    {
        t = 1;
    }
    else if (t < 0)
    {
        t = 0;
    }
    return Rect{
        math_rerp(a.pos, b.pos, t),
        math_rerp(a.siz, b.siz, t),
    };
}
