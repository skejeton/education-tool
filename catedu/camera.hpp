///
/// BRIEF: First-person camera controller.
///

#ifndef H_CAMERA_CATEDU
#define H_CAMERA_CATEDU

#include "math.hpp"

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
    Ray3 ray();
    void move(float sideways, float upward, float forward);
};

#endif // H_CAMERA_CATEDU
