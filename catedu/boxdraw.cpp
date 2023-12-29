#include "boxdraw.hpp"
#include "catedu/sys/sg_tricks.hpp"
#include "shaders.hxx"
#include <cassert>
#include <cstdlib>

#define INSTANCE_MAX 1024

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

struct Instance {
    Matrix4 transform;
    Vector2 uv_min;
    Vector2 uv_max;
    Vector2 size;
    Vector2 tile_count;
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

    sg_buffer_desc instance_buffer_desc = {};
    instance_buffer_desc.size = INSTANCE_MAX * sizeof(Instance);
    instance_buffer_desc.label = "cube-instance-buffer";
    instance_buffer_desc.usage = SG_USAGE_STREAM;
    result.instance_buffer = sg_make_buffer(instance_buffer_desc);

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

    pipeline_desc.layout.buffers[0].stride = sizeof(float) * 8;
    pipeline_desc.layout.buffers[1].stride = sizeof(Instance);
    pipeline_desc.layout.buffers[1].step_func = SG_VERTEXSTEP_PER_INSTANCE;
    // Vertex data
    pipeline_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3; // position
    pipeline_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT3; // normal
    pipeline_desc.layout.attrs[2].format = SG_VERTEXFORMAT_FLOAT2; // UV
    // Instance data
    pipeline_desc.layout.attrs[3].format = SG_VERTEXFORMAT_FLOAT4; // matrix row 1
    pipeline_desc.layout.attrs[3].buffer_index = 1;
    pipeline_desc.layout.attrs[4].format = SG_VERTEXFORMAT_FLOAT4; // matrix row 2
    pipeline_desc.layout.attrs[4].buffer_index = 1;
    pipeline_desc.layout.attrs[5].format = SG_VERTEXFORMAT_FLOAT4; // matrix row 3
    pipeline_desc.layout.attrs[5].buffer_index = 1;
    pipeline_desc.layout.attrs[6].format = SG_VERTEXFORMAT_FLOAT4; // matrix row 4
    pipeline_desc.layout.attrs[6].buffer_index = 1;
    pipeline_desc.layout.attrs[7].format = SG_VERTEXFORMAT_FLOAT2; // uv min
    pipeline_desc.layout.attrs[7].buffer_index = 1;
    pipeline_desc.layout.attrs[8].format = SG_VERTEXFORMAT_FLOAT2; // uv max
    pipeline_desc.layout.attrs[8].buffer_index = 1;
    pipeline_desc.layout.attrs[9].format = SG_VERTEXFORMAT_FLOAT2; // size
    pipeline_desc.layout.attrs[9].buffer_index = 1;
    pipeline_desc.layout.attrs[10].format = SG_VERTEXFORMAT_FLOAT2; // tile count
    pipeline_desc.layout.attrs[10].buffer_index = 1;



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
    result.bindings.vertex_buffers[1] = result.instance_buffer;
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

// TODO: Move this to the boxdraw instance
Instance instance_data[INSTANCE_MAX];
size_t instance_count = 0;

static void rflush(BoxdrawRenderer *renderer, boxdraw_vs_params_t vs_params)
{
    if (instance_count == 0)
        return;

    sg_range params_range = SG_RANGE(vs_params);
    sg_update_buffer(renderer->instance_buffer, {instance_data, instance_count * sizeof(Instance)});
    sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_boxdraw_vs_params,
                        &params_range);
    sg_draw(0, 36, instance_count);
    instance_count = 0;
}

void boxdraw_flush(BoxdrawRenderer *renderer, Matrix4 view_projection)
{
    const int width = sapp_width();
    const int height = sapp_height();

    sg_begin_default_pass(&renderer->pass_action, width, height);
    sg_apply_pipeline(renderer->pipeline);

    sg_image prev_image_id = {0};
    sg_sampler prev_sampler_id = {0};

    boxdraw_vs_params_t vs_params;
    for (size_t i = 0; i < renderer->commands_count; i++)
    {
        bool flush = false;
        BoxdrawCommand command = renderer->commands[i];

        Matrix4 model = create_box_transform(command.box);
        instance_data[instance_count++] = {
            view_projection * model,
            command.texture.uv_min(),
            command.texture.uv_max(),
            command.texture.size,
            command.texture.tile_count
        };

        vs_params.color_top = command.top_color;
        vs_params.color_bottom = command.bottom_color;
        vs_params.color_mul = {1, 1, 1, 1};

        if (command.texture.if_valid())
        {
            renderer->bindings.fs.images[0] = command.texture.sysid_texture;
            renderer->bindings.fs.samplers[0] = command.texture.sysid_sampler;
        }
        else
        {
            sg_tricks_get_white_texture(renderer->bindings.fs.images[0],
                                        renderer->bindings.fs.samplers[0]);
        }

        if (prev_image_id.id != command.texture.sysid_texture.id ||
            prev_sampler_id.id != command.texture.sysid_sampler.id)
        {
            sg_apply_bindings(renderer->bindings);
        } else if (instance_count >= INSTANCE_MAX) {
            assert(false);
        }

        prev_image_id = command.texture.sysid_texture;
        prev_sampler_id = command.texture.sysid_sampler;
    }

    rflush(renderer, vs_params);

    renderer->commands_count = 0;
    sg_end_pass();
}
