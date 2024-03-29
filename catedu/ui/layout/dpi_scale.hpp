#pragma once

#include "autolayout.hpp"

// Applies hinting for the DPI scale
static inline void fix_wacky_box(Rect &parent, Rect &child)
{
    float l = child.pos.x - parent.pos.x;
    float r = (parent.pos.x + parent.siz.x) - (child.pos.x + child.siz.x);
    float t = child.pos.y - parent.pos.y;
    float b = (parent.pos.y + parent.siz.y) - (child.pos.y + child.siz.y);
    float fl = floorf(l);
    float fr = floorf(r);
    float ft = floorf(t);
    float fb = floorf(b);

    if (ceilf(l) > l)
    {
        child.pos.x = parent.pos.x + fl;
    }
    if (ceilf(t) > t)
    {
        child.pos.y = parent.pos.y + ft;
    }
    if (ceilf(r) > r)
    {
        child.siz.x = parent.siz.x - (fl + fr);
    }
    if (ceilf(b) > b)
    {
        child.siz.y = parent.siz.y - (ft + fb);
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
        result->clip_box.pos *= scale;
        result->clip_box.siz *= scale;

        fix_wacky_box(result->border_box, result->base_box);

        result = result->next;
    }
}
