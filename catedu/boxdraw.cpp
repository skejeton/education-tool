#include "boxdraw.hpp"
#include "catedu/sys/sg_tricks.hpp"
#include "shaders.hxx"
#include <cassert>
#include <cstdlib>
#include <stdio.h>

// clang-format off
const float static cube_vertices[] = {
    // pos                normal    uv
    // +y
    -0.5, 0.5, 0.5,      0, 1, 0,  0, 1,
    -0.5, 0.5, -0.5,       0, 1, 0,  1, 1,
    0.5, 0.5, -0.5,        0, 1, 0,  1, 0,
    0.5, 0.5, 0.5,       0, 1, 0,  0, 0,

    // -x
    -0.5, 0.5, -0.5,      -1, 0, 0,  0, 0,
    -0.5, -0.5, -0.5,     -1, 0, 0,  0, 1,
    -0.5, -0.5, 0.5,    -1, 0, 0,  1, 1,
    -0.5, 0.5, 0.5,     -1, 0, 0,  1, 0,

    // +x
    0.5, 0.5, -0.5,        1, 0, 0,  0, 0,
    0.5, -0.5, -0.5,       1, 0, 0,  0, 1,
    0.5, -0.5, 0.5,      1, 0, 0,  1, 1,
    0.5, 0.5, 0.5,       1, 0, 0,  1, 0,

    // -z
    0.5, 0.5, -0.5,        0, 0,-1,  0, 0,
    0.5, -0.5, -0.5,       0, 0,-1,  0, 1,
    -0.5, -0.5, -0.5,      0, 0,-1,  1, 1,
    -0.5, 0.5, -0.5,       0, 0,-1,  1, 0,

    // +z
    0.5, 0.5, 0.5,       0, 0, 1,  0, 0,
    0.5, -0.5, 0.5,      0, 0, 1,  0, 1,
    -0.5, -0.5, 0.5,     0, 0, 1,  1, 1,
    -0.5, 0.5, 0.5,      0, 0, 1,  1, 0,

    // -y
    -0.5, -0.5, 0.5,     0,-1, 0,  0, 1,
    -0.5, -0.5, -0.5,      0,-1, 0,  1, 1,
    0.5, -0.5, -0.5,       0,-1, 0,  1, 0,
    0.5, -0.5, 0.5,      0,-1, 0,  0, 0
};

const static uint16_t cube_indices[] = {
    0, 1, 2,  0, 2, 3,       // +y
    6, 5, 4,  7, 6, 4,       // -x
    8, 9, 10,  8, 10, 11,    // +x
    14, 13, 12,  15, 14, 12, // -z
    16, 17, 18,  16, 18, 19, // +z
    22, 21, 20,  23, 22, 20  // -y
};
// clang-format on

BoxdrawRenderer boxdraw_create()
{
    BoxdrawRenderer result = {};
    result.commands =
        (BoxdrawCommand *)calloc(BOXDRAW_CMD_MAX, sizeof(BoxdrawCommand));

    result.shader =
        sg_make_shader(boxdraw_prog_shader_desc(sg_query_backend()));

    sg_buffer_desc vertex_buffer_desc = {};
    vertex_buffer_desc.data = SG_RANGE(cube_vertices);
    vertex_buffer_desc.label = "cube-vertices";
    vertex_buffer_desc.type = SG_BUFFERTYPE_VERTEXBUFFER;
    result.vertex_buffer = sg_make_buffer(vertex_buffer_desc);

    sg_buffer_desc index_buffer_desc = {};
    index_buffer_desc.data = SG_RANGE(cube_indices);
    index_buffer_desc.label = "cube-indices";
    index_buffer_desc.type = SG_BUFFERTYPE_INDEXBUFFER;
    result.index_buffer = sg_make_buffer(index_buffer_desc);

    sg_pipeline_desc pipeline_desc = {};
    pipeline_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3; // position
    pipeline_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT3; // normal
    pipeline_desc.layout.attrs[2].format = SG_VERTEXFORMAT_FLOAT2; // UV
    pipeline_desc.colors[0].blend.enabled = true;
    pipeline_desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
    pipeline_desc.colors[0].blend.dst_factor_rgb =
        SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    pipeline_desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
    pipeline_desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ZERO;
    pipeline_desc.shader = result.shader;
    pipeline_desc.index_type = SG_INDEXTYPE_UINT16;
    pipeline_desc.cull_mode = SG_CULLMODE_FRONT;
    pipeline_desc.depth.write_enabled = true;
    pipeline_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
    pipeline_desc.label = "cube-pipeline";
    result.pipeline = sg_make_pipeline(pipeline_desc);

    result.bindings.vertex_buffers[0] = result.vertex_buffer;
    result.bindings.index_buffer = result.index_buffer;

    // initial clear color
    result.pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
    result.pass_action.colors[0].store_action = SG_STOREACTION_STORE;
    result.pass_action.colors[0].clear_value = {0.0f, 0.0f, 0.5f, 1.0f};

    return result;
}

void boxdraw_destroy(BoxdrawRenderer *renderer)
{
    sg_destroy_buffer(renderer->vertex_buffer);
    sg_destroy_buffer(renderer->index_buffer);
    sg_destroy_pipeline(renderer->pipeline);
    sg_destroy_shader(renderer->shader);
}

void boxdraw_push(BoxdrawRenderer *renderer, BoxdrawCommand cmd)
{
    assert(renderer->commands_count < BOXDRAW_CMD_MAX);
    renderer->commands[renderer->commands_count++] = cmd;
}

/*
 * This function creates a transform for cube_vertices, so that it's positioned
 * at the correct corners of the specified box.
 */
Matrix4 create_box_transform(Box3 box)
{
    float width, height, depth;
    width = box.max.x - box.min.x;
    height = box.max.y - box.min.y;
    depth = box.max.z - box.min.z;

    Vector3 pos = box.min + Vector3{width, height, depth} / 2.0f;

    Matrix4 result =
        Matrix4::translate(pos) * Matrix4::scale_v({width, height, depth});

    return result;
}

void boxdraw_flush(BoxdrawRenderer *renderer, Matrix4 view_projection)
{
    const int width = sapp_width();
    const int height = sapp_height();

    sg_begin_default_pass(&renderer->pass_action, width, height);
    sg_apply_pipeline(renderer->pipeline);

    sg_image prev_image_id = {0};
    sg_sampler prev_sampler_id = {0};

    for (size_t i = 0; i < renderer->commands_count; i++)
    {
        BoxdrawCommand command = renderer->commands[i];

        Matrix4 model = create_box_transform(command.box);

        boxdraw_vs_params_t vs_params;
        vs_params.mvp = view_projection * model;
        vs_params.color_top = command.top_color;
        vs_params.color_bottom = command.bottom_color;
        vs_params.color_mul = {1, 1, 1, 1};

        if (command.texture.if_valid())
        {
            vs_params.uv_min = command.texture.uv_min();
            vs_params.uv_max = command.texture.uv_max();
            vs_params.size = command.texture.size;
            vs_params.tile_count = command.texture.tile_count;

            renderer->bindings.fs.images[0] = command.texture.sysid_texture;
            renderer->bindings.fs.samplers[0] = command.texture.sysid_sampler;
        }
        else
        {
            vs_params.uv_min = {0, 0};
            vs_params.uv_max = {1, 1};
            vs_params.size = {1, 1};

            sg_tricks_get_white_texture(renderer->bindings.fs.images[0],
                                        renderer->bindings.fs.samplers[0]);
        }

        if (prev_image_id.id != renderer->bindings.fs.images[0].id ||
            prev_sampler_id.id != renderer->bindings.fs.samplers[0].id)
        {
            sg_apply_bindings(renderer->bindings);
        }

        prev_image_id = command.texture.sysid_texture;
        prev_sampler_id = command.texture.sysid_sampler;

        sg_range params_range = SG_RANGE(vs_params);
        sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_boxdraw_vs_params,
                          &params_range);
        sg_draw(0, 36, 1);
    }

    renderer->commands_count = 0;
    sg_end_pass();
}
