#include "core.hpp"
#include "catedu/misc/util.hpp"
#include "catedu/rendering/2d/generate_mesh.hpp"
#include "catedu/shaders.hxx"
#include <assert.h>
#include <sokol/sokol_app.h>
#include <sokol/sokol_glue.h>

#define DEFAULT_SAMPLE_COUNT 256

static UiImage generate_image_from_data(int width, int height, const void *data)
{
    return ui_image_make_from_data({(void *)data, (size_t)(width * height * 4)},
                                   {width, height});
}

// Generates 2x2 image of magenta and black
static UiImage generate_test_image()
{
    return generate_image_from_data(
        2, 2,
        "\xFF\x00\xFF\xFF\x00\x00\x00\xFF\x00\x00\x00\xFF\xFF\x00\xFF\xFF");
}

// Generates 1x1 white image
static UiImage generate_white_image()
{
    return generate_image_from_data(1, 1, "\xFF\xFF\xFF\xFF");
}

UiBuffer generate_buffer(RenderGeo geo)
{
    sg_buffer_desc vertex_buffer_desc = {};
    sg_buffer_desc index_buffer_desc = {};

    // Fill UVs
    for (size_t i = 0; i < geo.vbuf_size; i += 4)
    {
        geo.vbuf[i + 2] = 1.0 - (geo.vbuf[i + 0] + 0.5f);
        geo.vbuf[i + 3] = 1.0 - (geo.vbuf[i + 1] + 0.5f);
    }

    vertex_buffer_desc.label = "ui-buf";
    index_buffer_desc.label = "ui-buf-i";
    vertex_buffer_desc.data = {geo.vbuf, geo.vbuf_size * sizeof(float)};
    index_buffer_desc.data = {geo.ibuf, geo.ibuf_size * sizeof(uint16_t)};
    vertex_buffer_desc.type = SG_BUFFERTYPE_VERTEXBUFFER;
    index_buffer_desc.type = SG_BUFFERTYPE_INDEXBUFFER;

    UiBuffer result = {};
    result.index_buffer = sg_make_buffer(index_buffer_desc);
    result.vertex_buffer = sg_make_buffer(vertex_buffer_desc);
    result.indices = geo.ibuf_size;

    return result;
}

UiRenderingCore UiRenderingCore::init()
{
    UiRenderingCore result = {};

    result.buffers[(int)UiBuffers::Ellipse] = generate_buffer(
        rendering_2d_generate_circle({4, 0}, DEFAULT_SAMPLE_COUNT));
    result.buffers[(int)UiBuffers::Squircle] =
        generate_buffer(rendering_2d_generate_squircle(
            {4, 0}, DEFAULT_SAMPLE_COUNT, 10, 0.5, 0.5));
    result.buffers[(int)UiBuffers::Rectangle] =
        generate_buffer(rendering_2d_generate_square({4, 0}));

    result.shader = sg_make_shader(ui_prog_shader_desc(sg_query_backend()));

    sg_pipeline_desc pipeline_desc = {};
    pipeline_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT2; // position
    pipeline_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT2; // uv
    pipeline_desc.colors[0].blend.enabled = true;
    pipeline_desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
    pipeline_desc.colors[0].blend.dst_factor_rgb =
        SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
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

    result.pass_action.colors[0].load_action = SG_LOADACTION_LOAD;
    result.pass_action.colors[0].store_action = SG_STOREACTION_STORE;

    result.white_image = result.alloc_image(generate_white_image());
    result.test_image = result.alloc_image(generate_test_image());

    return result;
}

void UiRenderingCore::deinit()
{
    for (size_t i = 0; i < (size_t)UiBuffers::Count_; i++)
    {
        sg_destroy_buffer(this->buffers[i].vertex_buffer);
        sg_destroy_buffer(this->buffers[i].index_buffer);
    }
    sg_destroy_pipeline(this->pipeline);
    sg_destroy_shader(this->shader);

    for (auto [id, _] : iter(this->images))
    {
        this->dealloc_image(id);
    }
}

void UiRenderingCore::begin_pipeline()
{
    this->pip_size = {sapp_widthf(), sapp_heightf()};

    sg_pass pass = {0};
    pass.action = this->pass_action;
    pass.swapchain = sglue_swapchain();
    sg_begin_pass(&pass);

    sg_apply_pipeline(this->pipeline);
}

void UiRenderingCore::end_pipeline()
{
    sg_end_pass();
}

UiImageId UiRenderingCore::alloc_image(UiImage image)
{
    return this->images.allocate(image);
}

void UiRenderingCore::dealloc_image(UiImageId id)
{
    UiImage *img = this->images.get(id);
    assert(img && "Trying to deallocate an invalid image id");
    sg_destroy_image(img->image);
    sg_destroy_sampler(img->sampler);
    this->images.remove(id);
}

UiImage *UiRenderingCore::get_image(UiImageId id)
{
    return this->images.get(id);
}

void UiRenderingCore::render_object(UiBrush brush)
{
    if (brush.image == NULL_ID)
    {
        brush.image = this->white_image;
    }

    UiImage *img = this->get_image(brush.image);
    assert(img && "Trying to render with an invalid image id");

    const float width = this->pip_size.x;
    const float height = this->pip_size.y;

    UiBuffer *buf = &this->buffers[(size_t)brush.buffer];

    this->bindings.vertex_buffers[0] = buf->vertex_buffer;
    this->bindings.index_buffer = buf->index_buffer;
    this->bindings.fs.images[0] = img->image;
    this->bindings.fs.samplers[0] = img->sampler;

    sg_apply_bindings(this->bindings);

    ui_vs_params_t vs_params;
    vs_params.mvp = matrix;
    vs_params.color_bottom = brush.color_bottom;
    vs_params.color_top = brush.color_top;
    rect_to_uv(brush.image_region_rect, vector2i_to_vector2(img->size),
               &vs_params.uv_min, &vs_params.uv_max);

    sg_range params_range = SG_RANGE(vs_params);

    sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_ui_vs_params, &params_range);
    sg_draw(0, buf->indices, 1);
}

UiImage ui_image_make_from_data(Buffer data, Vector2i size)
{
    sg_image_desc image_desc = {};
    image_desc.width = size.x;
    image_desc.height = size.y;
    image_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    image_desc.label = "ui-image";
    image_desc.data.subimage[0][0].ptr = data.data;
    image_desc.data.subimage[0][0].size = data.size;
    sg_sampler_desc sampler_desc = {};
    sampler_desc.min_filter = SG_FILTER_LINEAR;
    sampler_desc.mag_filter = SG_FILTER_LINEAR;
    sampler_desc.wrap_u = SG_WRAP_CLAMP_TO_EDGE;
    sampler_desc.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
    sampler_desc.label = "ui-sampler";

    return {sg_make_image(image_desc),
            sg_make_sampler(sampler_desc),
            {size.x, size.y}};
}
