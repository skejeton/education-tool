#include "edit_road.hpp"
#include <catedu/genobj/grid.hpp>
#include <catedu/genobj/road.hpp>

void EditRoad::show(UiUser &user, catedu::pbr::Renderer &renderer, World &world,
                    GenResources &gen_resources, Input &input, Camera &camera)
{
    Ray3 pointer_ray = camera.screen_to_world_ray(
        input.mouse_pos, {sapp_widthf(), sapp_heightf()});

    float t;
    ray3_vs_horizontal_plane(pointer_ray, -0.5, &t);

    Vector3 at = ray3_at(pointer_ray, t);
    Vector2i pointer = {(int)roundf(at.x), (int)roundf(at.z)};

    if (input.k[INPUT_MB_LEFT].pressed)
    {
        world.add_road(pointer.x, pointer.y);
    }
    else if (input.k[INPUT_MB_RIGHT].pressed)
    {
        world.remove_object(pointer.x, pointer.y);
    }

    if (world.can_place_road(pointer.x, pointer.y))
    {
        GeneratedObject road = genmesh_generate_road();
        genobj_render_object(
            renderer, gen_resources, road,
            Matrix4::translate({(float)pointer.x, 0, (float)pointer.y}));
    }

    GeneratedObject grid = genmesh_generate_grid(16, 16);
    genobj_render_object(
        renderer, gen_resources, grid,
        Matrix4::translate({(float)pointer.x, 0, (float)pointer.y}));
}
