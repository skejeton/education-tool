#include "pass.hpp"

UiRenderingPass UiRenderingPass::begin(UiRenderingCore *core)
{
    UiRenderingPass pass = {};
    pass.core = core;
    core->begin_pipeline();
    return pass;
}

void UiRenderingPass::end()
{
    this->core->end_pipeline();
}

void UiRenderingPass::render_brush(UiBrush brush, Rect rect)
{
    this->core->render_object(rect, brush);
}
