#include "bounds.hpp"
#include "catedu/core/math/math.hpp"
#include "construct.hpp"

GeneratedObject genmesh_generate_bounds(RectI bounds, Color color)
{
    GeneratedObject obj = {};

    obj.push_colored_box(
        box3_translate(
            construct_box(Baseline::Bottom, {(float)bounds.siz.x + 0.5f, 0.5,
                                             (float)bounds.siz.y + 0.5f}),
            {(float)bounds.pos.x + bounds.siz.x / 2.0f, 0,
             (float)bounds.pos.y + bounds.siz.y / 2.0f}),
        color);

    return obj;
}
