///
/// BRIEF: Rendering core for UI
///

#pragma once
#include "catedu/core/math/math.hpp"
#include "catedu/core/memory/buffer.hpp"
#include "catedu/core/storage/table.hpp"
#include "catedu/rendering/color.hpp"
#include "sokol/sokol_gfx.h"

struct UiFilter
{
    Color color_top;
    Color color_bottom;
};

struct UiBuffer
{
    sg_buffer vertex_buffer;
    sg_buffer index_buffer;
    size_t indices;
};

typedef TableId UiImageId;

struct UiImage
{
    sg_image image;
    sg_sampler sampler;
    Vector2i size;
};

struct UiBrush
{
    Color color_bottom;
    Color color_top;
    UiImageId image;
    Rect image_region_rect;
    Vector4 radius;
    Vector4 gaps_x;
    Vector4 gaps_y;
};

struct UiRenderingCore
{
    UiBuffer buffer;
    sg_pass_action pass_action;
    sg_bindings bindings;
    sg_pipeline pipeline;
    sg_shader shader;
    UiImageId white_image;
    UiImageId test_image;
    Table<UiImage> images;
    Matrix4 matrix;
    Rect rect;
    UiFilter filter;

    // Per pipeline
    Vector2 pip_size;

    static UiRenderingCore init();
    void deinit();
    void begin_pipeline();
    void end_pipeline();

    UiImageId alloc_image(UiImage image);
    void dealloc_image(UiImageId id);
    UiImage *get_image(UiImageId id);

    void render_object(UiBrush brush);
    void begin_scissor(Rect rect);
    void end_scissor();
};

UiImage ui_image_make_from_data(Buffer data, Vector2i size);
