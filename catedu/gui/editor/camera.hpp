#pragma once
#include <catedu/rendering/3d/camera.hpp>
#include <catedu/sys/input.hpp>

struct EditorCamera
{
    Camera cam;
    float zoom;
    float zoom_target;

    static EditorCamera create();
    void lockin(Vector3 pos, float rotation);
    void follow(Vector3 pos, float rotation, float zoom_target = 4);
    void update(Vector2i window_size);
    void handle_controls(Input &input);
};
