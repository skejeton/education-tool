#include "edit_basic.hpp"
#include "catedu/gui/editor/world.hpp"
#include <catedu/genobj/grid.hpp>
#include <catedu/genobj/player.hpp>
#include <catedu/genobj/road.hpp>
#include <catedu/genobj/wall.hpp>

void EditBasic::show(UiUser &user, catedu::pbr::Renderer &renderer,
                     Dispatcher &disp, GenResources &gen_resources,
                     Input &input, Camera &camera, Object::Type type)
{
    Ray3 pointer_ray = camera.screen_to_world_ray(
        input.mouse_pos, {sapp_widthf(), sapp_heightf()});

    float t;
    ray3_vs_horizontal_plane(pointer_ray, -0.5, &t);

    Vector3 at = ray3_at(pointer_ray, t);
    Vector2 pointer = {floorf(at.x), floorf(at.z)};

    if (input.k[INPUT_MB_LEFT].pressed)
    {
        disp.place_object({type, 0, pointer.x, pointer.y});
    }

    if (disp.world.current->can_place_objtype(type, pointer.x, pointer.y))
    {
        GeneratedObject obj;

        switch (type)
        {
        case Object::Type::Player:
            obj = genmesh_generate_player();
            break;
        default:
            assert(false);
            break;
        }

        genobj_render_object(renderer, gen_resources, obj,
                             Matrix4::translate({pointer.x, 0, pointer.y}));
    }

    GeneratedObject grid = genmesh_generate_grid(16, 16);
    genobj_render_object(renderer, gen_resources, grid,
                         Matrix4::translate({pointer.x, 0, pointer.y}));
}
