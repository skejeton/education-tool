#include "camera.hpp"
#include <stdio.h>

float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

EditorCamera EditorCamera::create()
{
    EditorCamera camera = {};
    camera.cam = Camera::init(45);
    camera.cam.rotate(0, -45);
    camera.zoom_target = 6;
    return camera;
}

void EditorCamera::follow(Vector3 pos, float rotation)
{
    cam.move(0, -(zoom * zoom), (zoom * zoom));
    cam.yaw = lerp(cam.yaw, rotation, 0.2);
    zoom_target = 4;
    cam.position.x = lerp(cam.position.x, pos.x, 0.2);
    cam.position.z = lerp(cam.position.z, pos.z, 0.2);
    cam.move(0, 0, 0);
    cam.move(0, (zoom * zoom), -(zoom * zoom));
}

void EditorCamera::handle_controls(Input &input, Vector2i window_size)
{
    cam.set_aspect((float)window_size.x / (float)window_size.y);

    cam.move(0, -(zoom * zoom), (zoom * zoom));

    if (input.k[INPUT_MB_MIDDLE].held)
    {
        if (input.k[INPUT_MB_RIGHT].held)
        {
            cam.rotate(input.mouse_delta.x / 5, 0);
        }
        else
        {
            cam.move(-input.mouse_delta.x / 10, 0, input.mouse_delta.y / 10);
        }
    }

    zoom_target -= input.mouse_wheel;
    zoom_target = clamp<float>(zoom_target, 2, 8);
    zoom = lerp(zoom, zoom_target, 0.2);

    cam.move(0, (zoom * zoom), -(zoom * zoom));
}
