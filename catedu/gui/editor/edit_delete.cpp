#include "edit_delete.hpp"
#include "catedu/sys/input.hpp"

void EditDelete::show(UiUser &user, catedu::pbr::Renderer &renderer,
                      Dispatcher &disp, GenResources &gen_resources,
                      Input &input, Camera &camera)
{

    Ray3 pointer_ray = camera.screen_to_world_ray(
        input.mouse_pos, {sapp_widthf(), sapp_heightf()});

    float t;
    ray3_vs_horizontal_plane(pointer_ray, 0, &t);

    Vector3 at = ray3_at(pointer_ray, t);
    Vector2 pointer = {roundf(at.x), roundf(at.z)};

    if (input.k[INPUT_MB_LEFT].pressed)
    {
        disp.remove_object(pointer.x, pointer.y);
    }
}
