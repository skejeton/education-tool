///
/// BRIEF: Rendering core for UI
///

#ifndef HPP_CATEDU_UI_RENDERING_CORE
#define HPP_CATEDU_UI_RENDERING_CORE

#include "sokol/sokol_gfx.h"
#include "catedu/math.hpp"
#include "catedu/table.hpp"

struct UiBuffer {
    sg_buffer vertex_buffer;
    sg_buffer index_buffer;
    size_t indices;
};

enum class UiBuffers {
    Ellipse,
    Squircle,
    Rectangle,
    Count_
};

typedef TableId UiImageId;

struct UiImage {
    sg_image image;
    sg_sampler sampler;
    Vector2i size;
};

struct UiBrush {
    UiBuffers buffer;
    Vector4 color_bottom;
    Vector4 color_top;
    UiImageId image;
};

struct UiRenderingCore {
    UiBuffer buffers[(size_t)UiBuffers::Count_];
    sg_pass_action pass_action;
    sg_bindings bindings;
    sg_pipeline pipeline;
    sg_shader shader;
    UiImage white_image;
    UiImage test_image;
    Table<UiImage> images;

    // Per pipeline
    Vector2 pip_size;

    static UiRenderingCore init();
    void deinit();
    void begin_pipeline();
    void end_pipeline();

    UiImageId alloc_image(sg_image_desc desc, sg_sampler_desc sampler_desc);
    void dealloc_image(UiImageId id);
    UiImage *get_image(UiImageId id);

    void render_object(Rect rect, UiBrush brush);
};

#endif // HPP_CATEDU_UI_RENDERING_CORE
