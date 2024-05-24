#pragma once
#include <catedu/rendering/3d/camera.hpp>
#include <catedu/sys/input.hpp>

struct EditorCamera
{
    Camera cam;
    Vector2 focus_point;
    float zoom;
    float zoom_target;
    float yaw;
    float pitch;

    void handle_controls(Input &input, Vector2i window_size);
};
