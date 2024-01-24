#pragma once

#include "autolayout.hpp"

static inline void fix_wacky_box(Rect &parent, Rect &child)
{
    float l = child.pos.x - parent.pos.x;
    float r = (parent.pos.x + parent.siz.x) - (child.pos.x + child.siz.x);
    float t = child.pos.y - parent.pos.y;
    float b = (parent.pos.y + parent.siz.y) - (child.pos.y + child.siz.y);
    float rx = 0;
    float ry = 0;

    if (ceilf(l) > l)
    {
        child.pos.x = parent.pos.x + floorf(l);
        rx += ceilf(l) - l;
    }
    if (ceilf(t) > t)
    {
        child.pos.y = parent.pos.y + floorf(t);
        ry += ceilf(t) - t;
    }
    if (ceilf(r) > r)
    {
        child.siz.x = parent.siz.x - floorf(r + rx);
    }
    if (ceilf(b) > b)
    {
        child.siz.y = parent.siz.y - floorf(b + ry);
    }
}

static inline void dpi_rescale_autolayout_result(AutoLayoutResult *result,
                                                 float scale)
{
    while (result)
    {
        result->base_box.pos *= scale;
        result->base_box.siz *= scale;
        result->border_box.pos *= scale;
        result->border_box.siz *= scale;
        result->margin_box.pos *= scale;
        result->margin_box.siz *= scale;
        result->padding_box.pos *= scale;
        result->padding_box.siz *= scale;

        fix_wacky_box(result->border_box, result->base_box);

        result = result->next;
    }
}
