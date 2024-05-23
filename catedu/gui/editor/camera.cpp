#include "camera.hpp"
#include <stdio.h>

void EditorCamera::handle_controls(Input &input, Vector2i window_size)
{
    if (input.k[INPUT_MB_MIDDLE].held)
    {
        if (input.k[INPUT_MB_RIGHT].held)
        {
            yaw += input.mouse_delta.x / 5;
        }
        else
        {
            focus_point.x -= input.mouse_delta.x / 20;
            focus_point.y += input.mouse_delta.y / 20;
        }
    }
    zoom += input.mouse_wheel;

    cam = Camera::init(45);
    cam.set_aspect((float)window_size.x / (float)window_size.y);
    cam.rotate(yaw, 0);
    cam.move(focus_point.x, 10, focus_point.y);
    cam.rotate(0, -55);
    cam.move(0, -zoom, zoom);
}
