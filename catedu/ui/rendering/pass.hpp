///
/// BRIEF: Rendering pass for UI
///
#pragma once
#include "core.hpp"
#include "transform.hpp"

struct UiRenderingPass
{
    UiTransformer transformer;
    UiRenderingCore *core;

    static UiRenderingPass begin(UiRenderingCore *core);
    void end();

    void push_transform(UiTransform transform);
    void pop_transform();

    Vector2 transform_point(Vector2 point);

    void render_brush(UiBrush brush);
};
