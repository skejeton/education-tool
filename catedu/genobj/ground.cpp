#include "ground.hpp"
#include "construct.hpp"

GeneratedObject genmesh_generate_ground(bool indoor)
{
    if (indoor)
    {
        GeneratedObject object = {};

        Box3 box = construct_box(Baseline::Bottom, {24, 5, 24});
        box = box3_translate(box, {0, -5, 0});

        object.push_colored_box(box, Color::hex(0x666666FF));

        object.push_colored_box(
            box3_translate(construct_box(Baseline::Bottom, {2, 4, 0.1}),
                           {0, 0, -12}),
            Color::hex(0x770000FF));

        return object;
    }

    GeneratedObject object = {};

    Box3 box = construct_box(Baseline::Bottom, {256, 100, 256});
    box = box3_translate(box, {0, -100, 0});

    object.push_colored_box(box, Color::hex(0x009900FF));

    return object;
}
