#include "edit_delete.hpp"
#include "catedu/genobj/bounds.hpp"
#include "catedu/genobj/generator.hpp"
#include "catedu/genobj/render.hpp"
#include "catedu/sys/input.hpp"

void EditDelete::show(UiPass &user, Renderer &renderer, Dispatcher &disp,
                      GenResources &gen_resources, Input &input, Camera &camera)
{

    Ray3 pointer_ray = camera.screen_to_world_ray(
        input.mouse_pos, {sapp_widthf(), sapp_heightf()});

    float t;
    ray3_vs_horizontal_plane(pointer_ray, 0, &t);

    Vector3 at = ray3_at(pointer_ray, t);
    Vector2 pointer = {roundf(at.x), roundf(at.z)};

    Object *obj = disp.world.current->get_object_at(pointer.x, pointer.y);
    if (obj)
    {
        RectI bounds = disp.world.current->object_bounds(*obj);

        GeneratedObject obj =
            genmesh_generate_bounds(bounds, Color::hex(0xFF0000FF));

        genobj_render_object(renderer, gen_resources, obj);

        if (input.k[INPUT_MB_LEFT].pressed)
        {
            disp.remove_object(pointer.x, pointer.y);
        }
    }
}
