#include "pass.hpp"
#include "sokol/sokol_app.h"

UiRenderingPass UiRenderingPass::begin(UiRenderingCore *core)
{
    UiRenderingPass pass = {};
    pass.core = core;
    core->begin_pipeline();
    pass.transformer =
        UiTransformer::init(core, {sapp_widthf(), sapp_heightf()});

    UiTransform transform = {};
    transform.scale = {1, 1};
    transform.base = {0, 0, sapp_widthf(), sapp_heightf()};

    pass.push_transform(transform);
    return pass;
}

void UiRenderingPass::end()
{
    this->pop_transform();
    this->core->end_pipeline();
}

void UiRenderingPass::push_transform(UiTransform transform)
{
    this->transformer.push(transform);
}

void UiRenderingPass::pop_transform()
{
    this->transformer.pop();
}

Vector2 UiRenderingPass::transform_point(Vector2 point)
{
    return this->transformer.transform_point(point);
}

void UiRenderingPass::render_brush(UiBrush brush)
{
    this->core->render_object(brush);
}
