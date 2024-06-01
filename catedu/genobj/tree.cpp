#include "tree.hpp"
#include "catedu/genobj/construct.hpp"
#include "catedu/genobj/generator.hpp"

GeneratedObject genmesh_generate_tree()
{
    GeneratedObject obj = {};
    obj.push_colored_box(construct_box(Baseline::Bottom, {0.4, 2, 0.4}),
                         Color::hex(0xAA0000FF), true);
    obj.push_colored_box(
        box3_translate(construct_box(Baseline::Bottom, {1, 2, 1}), {0, 2, 0}),
        Color::hex(0x00FF77FF), true);
    return obj;
}
