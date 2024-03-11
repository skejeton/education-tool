///
/// BRIEF: Camera controller that uses Input structure.
///

#include "catedu/rendering/3d/camera.hpp"
#include "catedu/sys/input.hpp"

inline void camera_input_top_view_apply(Camera *camera, Input *input,
                                        bool fly = true)
{
    float speed = sapp_frame_duration() * 10;

    if (input->k[SAPP_KEYCODE_W].held)
    {
        camera->move(0, 0, speed);
    }

    if (input->k[SAPP_KEYCODE_S].held)
    {
        camera->move(0, 0, -speed);
    }

    if (input->k[SAPP_KEYCODE_A].held)
    {
        camera->move(-speed, 0, 0);
    }

    if (input->k[SAPP_KEYCODE_D].held)
    {
        camera->move(speed, 0, 0);
    }
}

inline void camera_input_apply(Camera *camera, Input *input, bool fly = true)
{
    camera->rotate(input->mouse_delta.x / 6, -input->mouse_delta.y / 6);
    float speed = sapp_frame_duration() * 10;

    if (fly)
    {
        if (input->k[SAPP_KEYCODE_LEFT_SHIFT].held)
        {
            camera->move(0, -speed, 0);
        }

        if (input->k[SAPP_KEYCODE_SPACE].held)
        {
            camera->move(0, speed, 0);
        }
    }

    if (input->k[SAPP_KEYCODE_W].held)
    {
        camera->move(0, 0, speed);
    }

    if (input->k[SAPP_KEYCODE_S].held)
    {
        camera->move(0, 0, -speed);
    }

    if (input->k[SAPP_KEYCODE_A].held)
    {
        camera->move(-speed, 0, 0);
    }

    if (input->k[SAPP_KEYCODE_D].held)
    {
        camera->move(speed, 0, 0);
    }
}
