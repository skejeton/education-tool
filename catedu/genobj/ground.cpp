#include "ground.hpp"
#include "construct.hpp"

GeneratedObject genmesh_generate_ground()
{
    GeneratedObject object = {};

    Box3 box = construct_box(Baseline::Bottom, {100, 100, 100});
    box = box3_translate(box, {0, -100, 0});

    object.push_colored_box(box, Color::hex(0x009900FF));

    return object;
}
