#include "make_brush.hpp"

UiMakeBrush UiMakeBrush::make_plain_brush()
{
    UiMakeBrush maker = {};
    maker.brush.radius = {1, 1, 1, 1};
    maker.brush.gaps_x = {1, 1, 1, 1};
    maker.brush.gaps_y = {1, 1, 1, 1};
    return maker;
}

UiMakeBrush UiMakeBrush::make_image_brush(UiRenderingCore *core,
                                          UiImageId image)
{
    UiMakeBrush maker = {};
    maker.brush.radius = {1, 1, 1, 1};
    maker.brush.gaps_x = {1, 1, 1, 1};
    maker.brush.gaps_y = {1, 1, 1, 1};
    maker.brush.image = image;
    maker.brush.color_bottom = 0xFFFFFFFF;
    maker.brush.color_top = 0xFFFFFFFF;
    UiImage *image_struct = core->get_image(image);
    if (image_struct)
    {
        maker.brush.image_region_rect = {
            {0, 0}, vector2i_to_vector2(image_struct->size)};
    }
    return maker;
}

UiBrush UiMakeBrush::make_solid(Color color)
{
    return UiMakeBrush::make_plain_brush().with_solid(color).build();
}

UiBrush UiMakeBrush::make_gradient(Color color_bottom, Color color_top)
{
    return UiMakeBrush::make_plain_brush()
        .with_gradient(color_bottom, color_top)
        .build();
}

UiMakeBrush &UiMakeBrush::squircle(float radius)
{
    brush.radius = {4, 4, 4, 4};
    brush.gaps_x = {radius, radius, radius, radius};
    brush.gaps_y = {radius, radius, radius, radius};
    return *this;
}

UiMakeBrush &UiMakeBrush::squircle(float radiusx, float radiusy)
{
    brush.radius = {4, 4, 4, 4};
    brush.gaps_x = {radiusx, radiusx, radiusx, radiusx};
    brush.gaps_y = {radiusy, radiusy, radiusy, radiusy};
    return *this;
}

UiMakeBrush &UiMakeBrush::with_solid(Color color)
{
    this->brush.color_bottom = color;
    this->brush.color_top = color;
    return *this;
}

UiMakeBrush &UiMakeBrush::with_gradient(Color color_bottom, Color color_top)
{
    this->brush.color_bottom = color_bottom;
    this->brush.color_top = color_top;
    return *this;
}

UiMakeBrush &UiMakeBrush::with_image_region(Rect region)
{
    this->brush.image_region_rect = region;
    return *this;
}

UiBrush UiMakeBrush::build()
{
    return this->brush;
}
