#ifndef HPP_CATEDU_CORE_UI_RENDERING
#define HPP_CATEDU_CORE_UI_RENDERING

#include "sokol/sokol_gfx.h"

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

    static UiRendering init();
    void deinit();

    void render_object(UiBuffers buffer);
};

#endif
