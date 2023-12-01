///
/// BRIEF: Rendering pass for UI
///
#ifndef HPP_CATEDU_UI_RENDERING_PASS
#define HPP_CATEDU_UI_RENDERING_PASS

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

#endif // HPP_CATEDU_UI_RENDERING_PASS
