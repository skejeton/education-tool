///
/// BRIEF: Rendering core for UI
///

#pragma once
#include "catedu/core/math/math.hpp"
#include "catedu/core/memory/buffer.hpp"
#include "catedu/core/storage/table.hpp"
#include "sokol/sokol_gfx.h"

struct UiBuffer
{
    sg_buffer vertex_buffer;
    sg_buffer index_buffer;
    size_t indices;
};

enum class UiBuffers
{
    Ellipse,
    Squircle,
    Rectangle,
    Count_
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
    UiBuffers buffer;
    Vector4 color_bottom;
    Vector4 color_top;
    UiImageId image;
    Rect image_region_rect;
};

struct UiRenderingCore
{
    UiBuffer buffers[(size_t)UiBuffers::Count_];
    sg_pass_action pass_action;
    sg_bindings bindings;
    sg_pipeline pipeline;
    sg_shader shader;
    UiImageId white_image;
    UiImageId test_image;
    Table<UiImage> images;
    Matrix4 matrix;
    Rect rect;

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
};

UiImage ui_image_make_from_data(Buffer data, Vector2i size);
