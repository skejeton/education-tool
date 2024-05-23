#include "road.hpp"
#include "construct.hpp"

GeneratedObject genmesh_generate_road()
{
    GeneratedObject object = {};

    object.push_colored_box(construct_box(Baseline::Bottom, {4, 0.1, 4}),
                            Color::hex(0x333333FF));

    return object;
}
