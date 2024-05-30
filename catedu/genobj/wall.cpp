#include "wall.hpp"
#include "construct.hpp"

GeneratedObject genmesh_generate_wall()
{
    GeneratedObject obj = {};
    obj.push_colored_box(
        box3_translate(construct_box(Baseline::Bottom, {1, 2, 1}),
                       {0.5, 0.0, 0.5}),
    Color::hex(0x7777AAFF));
    return obj;
}
