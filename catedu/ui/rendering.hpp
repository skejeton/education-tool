#ifndef HPP_CATEDU_CORE_UI_RENDERING
#define HPP_CATEDU_CORE_UI_RENDERING

#include "sokol/sokol_gfx.h"
#include "catedu/math.hpp"

struct UiBuffer {
    sg_buffer vertex_buffer;
    sg_buffer index_buffer;
    size_t indices;
};

enum class UiBuffers {
    Circle,
    Squircle,
    Rectangle,
    Count_
};

struct UiRendering {
    UiBuffer buffers[(size_t)UiBuffers::Count_];
    sg_pass_action pass_action;
    sg_bindings bindings;
    sg_pipeline pipeline;
    sg_shader shader;

    // Per pipeline
    Vector2 pip_size;

    static UiRendering init();
    void deinit();

    void begin_pipeline();
    void end_pipeline();
    void render_object(UiBuffers buffer, Rect rect, Vector4 color);
};

#endif
