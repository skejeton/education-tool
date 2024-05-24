#include "camera.hpp"
#include <stdio.h>

float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

void EditorCamera::handle_controls(Input &input, Vector2i window_size)
{
    if (input.k[INPUT_MB_MIDDLE].held)
    {
        if (input.k[INPUT_MB_RIGHT].held)
        {
            yaw += input.mouse_delta.x / 5;
            pitch += input.mouse_delta.y / 5;
        }
        else
        {
            focus_point.x -= input.mouse_delta.x / 20;
            focus_point.y += input.mouse_delta.y / 20;
        }
    }
    zoom_target -= input.mouse_wheel;
    zoom_target = clamp<float>(zoom_target, 0, 6);
    zoom = lerp(zoom, zoom_target, 0.5);

    cam = Camera::init(45);
    cam.set_aspect((float)window_size.x / (float)window_size.y);
    cam.rotate(yaw, 0);
    cam.move(focus_point.x, 10, focus_point.y);
    cam.rotate(0, -55);
    cam.move(0, (zoom * zoom), -(zoom * zoom));
}
