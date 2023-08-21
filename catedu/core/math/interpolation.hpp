#ifndef HPP_CATEDU_CORE_MATH_INTERPOLATION
#define HPP_CATEDU_CORE_MATH_INTERPOLATION

float math_lerp(float a, float b, float t)
{
    return (b-a)*t+a;
}

#endif

