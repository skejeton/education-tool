#include "deleter.hpp"
#include "catedu/genobj/generator.hpp"
#include "construct.hpp"

GeneratedObject genmesh_generate_deleter()
{
    GeneratedObject obj = {};
    obj.push_colored_box(construct_box(Baseline::Bottom, {10, 2, 2}),
                         Color::hex(0xFF0000FF));
    obj.push_colored_box(construct_box(Baseline::Bottom, {2, 2, 10}),
                         Color::hex(0xFF0000FF));
    return obj;
}
