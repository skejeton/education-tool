///
/// BRIEF: Rendering pass for UI
///
#ifndef HPP_CATEDU_UI_RENDERING_PASS
#define HPP_CATEDU_UI_RENDERING_PASS

#include "core.hpp"

struct UiRenderingPass {
    UiRenderingCore *core;

    static UiRenderingPass begin(UiRenderingCore *core);
    void end();

    void render_brush(UiBrush brush, Rect rect);
};

#endif // HPP_CATEDU_UI_RENDERING_PASS
