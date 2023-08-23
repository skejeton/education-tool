#include "rendering.hpp"
#include "catedu/rendering/2d/generate_mesh.hpp"
#include "catedu/shaders.hxx"
#include <sokol/sokol_app.h>
#include "catedu/core/math/interpolation.hpp"

#define DEFAULT_SAMPLE_COUNT 128

UiBuffer generate_buffer(RenderGeo geo) {
    sg_buffer_desc vertex_buffer_desc = {};
    sg_buffer_desc index_buffer_desc = {};

    for (size_t i = 0; i < geo.vbuf_size; i += 3) {
        geo.vbuf[i+2] = geo.vbuf[i+1]+0.5f;
    }

    vertex_buffer_desc.label = "ui-buf";
    index_buffer_desc.label = "ui-buf-i";
    vertex_buffer_desc.data = {geo.vbuf, geo.vbuf_size*sizeof(float)};
    index_buffer_desc.data = {geo.ibuf, geo.ibuf_size*sizeof(uint16_t)};
    vertex_buffer_desc.type = SG_BUFFERTYPE_VERTEXBUFFER;
    index_buffer_desc.type = SG_BUFFERTYPE_INDEXBUFFER;

    UiBuffer result = {};
    result.index_buffer = sg_make_buffer(index_buffer_desc);
    result.vertex_buffer = sg_make_buffer(vertex_buffer_desc);
    result.indices = geo.ibuf_size;

    return result;
}

UiRendering UiRendering::init()
{
    UiRendering result = {};

    result.buffers[(int)UiBuffers::Circle] = generate_buffer(rendering_2d_generate_circle({3, 0}, DEFAULT_SAMPLE_COUNT));
    result.buffers[(int)UiBuffers::Squircle] = generate_buffer(rendering_2d_generate_squircle({3, 0}, DEFAULT_SAMPLE_COUNT, 8, 0.5, 0.5));
    result.buffers[(int)UiBuffers::Rectangle] = generate_buffer(rendering_2d_generate_square({3, 0}));

    result.shader = sg_make_shader(ui_prog_shader_desc(sg_query_backend()));

    sg_pipeline_desc pipeline_desc = {};
    pipeline_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT2; // position
    pipeline_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT; // gradient
    pipeline_desc.colors[0].blend.enabled = true;
    pipeline_desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
    pipeline_desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pipeline_desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    pipeline_desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ZERO;
    pipeline_desc.shader = result.shader;
    pipeline_desc.index_type = SG_INDEXTYPE_UINT16;
    pipeline_desc.cull_mode = SG_CULLMODE_NONE;
    pipeline_desc.depth.write_enabled = true;
    pipeline_desc.depth.pixel_format = SG_PIXELFORMAT_DEPTH_STENCIL;
    pipeline_desc.depth.compare = SG_COMPAREFUNC_ALWAYS;
    pipeline_desc.label = "ui-pipeline";

    result.pipeline = sg_make_pipeline(pipeline_desc);


    result.bindings.vertex_buffers[0] = result.buffers[0].vertex_buffer;
    result.bindings.index_buffer = result.buffers[0].index_buffer;

    result.pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
    result.pass_action.colors[0].store_action = SG_STOREACTION_STORE;
    result.pass_action.colors[0].clear_value = {0.5, 0.5, 0.5, 1.0};

    return result;
}

void UiRendering::deinit()
{
    for (size_t i = 0; i < (size_t)UiBuffers::Count_; i++) {
        sg_destroy_buffer(this->buffers[i].vertex_buffer);
        sg_destroy_buffer(this->buffers[i].index_buffer);
    }
    sg_destroy_pipeline(this->pipeline);
    sg_destroy_shader(this->shader);
}

void UiRendering::begin_pipeline()
{
    this->pip_size = {sapp_widthf(), sapp_heightf()};

    sg_begin_default_pass(&this->pass_action, this->pip_size.x, this->pip_size.y);
    sg_apply_pipeline(this->pipeline);
}

void UiRendering::end_pipeline()
{
    sg_end_pass();
}

void UiRendering::render_object(UiBuffers buffer, Rect rect, Vector4 color)
{
    const float width = this->pip_size.x;
    const float height = this->pip_size.y;

    UiBuffer *buf = &this->buffers[(size_t)buffer];

    this->bindings.vertex_buffers[0] = buf->vertex_buffer;
    this->bindings.index_buffer = buf->index_buffer;

    sg_apply_bindings(this->bindings);

    Matrix4 rect_mvp = Matrix4::identity();
    rect_mvp = rect_mvp * Matrix4::scale_v({2.0f/width, 2.0f/height, 0});
    rect_mvp = rect_mvp * Matrix4::translate({-(-rect.pos.x-(rect.siz.x-width)/2), -rect.pos.y-(rect.siz.y-height)/2, 0});
    rect_mvp = rect_mvp * Matrix4::scale_v({rect.siz.x, rect.siz.y, 0});

    ui_vs_params_t vs_params;
    vs_params.mvp = rect_mvp;
    // light gray to white
    vs_params.color_top = Vector4{0.8f, 0.8f, 0.8f, 1.0f}*color;
    vs_params.color_bottom = Vector4{1.0f, 1.0f, 1.0f, 1.0f}*color;


    sg_range params_range = SG_RANGE(vs_params);

    sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_ui_vs_params, &params_range);
    sg_draw(0, buf->indices, 1);
}



