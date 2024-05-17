#pragma once
#include "catedu/core/math/math.hpp"

enum class Baseline
{
    Center,
    Bottom,
};

static inline Box3 construct_box(Baseline baseline, Vector3 size)
{
    Box3 box = box3_extrude_from_point_volume({0, 0, 0}, size);

    switch (baseline)
    {
    case Baseline::Center:
        break;
    case Baseline::Bottom:
        box = box3_translate(box, {0, size.y / 2, 0});
        break;
    }

    return box;
}
