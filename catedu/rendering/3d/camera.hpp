///
/// BRIEF: First-person camera controller.
///

#pragma once
#include "catedu/core/math/math.hpp"

struct Camera
{
    float fov_deg;
    float aspect;
    float yaw, pitch;
    Vector3 position;
    Matrix4 vp;

    static Camera init(float fov_deg);
    void set_aspect(float aspect);
    void rotate(float yaw, float pitch);
    void rotate_around(Vector3 point, float yaw, float pitch);
    Ray3 ray();
    void move(float sideways, float upward, float forward);
};
