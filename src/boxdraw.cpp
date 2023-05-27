#include "boxdraw.hpp"
#include "shaders.hxx"
#include <cassert>
#include <cstdlib>

static float cube_vertices[] = {
  -1.0, -1.0, -1.0,   1.0,
  1.0, -1.0, -1.0,    1.0,
  1.0,  1.0, -1.0,    0.0,
  -1.0,  1.0, -1.0,   0.0,

  -1.0, -1.0,  1.0,   1.0,
  1.0, -1.0,  1.0,    1.0,
  1.0,  1.0,  1.0,    0.0,
  -1.0,  1.0,  1.0,   0.0,

  -1.0, -1.0, -1.0,   1.0,
  -1.0,  1.0, -1.0,   0.0, 
  -1.0,  1.0,  1.0,   0.0, 
  -1.0, -1.0,  1.0,   1.0, 

  1.0, -1.0, -1.0,    1.0, 
  1.0,  1.0, -1.0,    0.0,
  1.0,  1.0,  1.0,    0.0, 
  1.0, -1.0,  1.0,    1.0, 

  -1.0, -1.0, -1.0,   1.0,
  -1.0, -1.0,  1.0,   1.0,
  1.0, -1.0,  1.0,    1.0, 
  1.0, -1.0, -1.0,    1.0,

  -1.0,  1.0, -1.0,   0.0,
  -1.0,  1.0,  1.0,   0.0,
  1.0,  1.0,  1.0,    0.0, 
  1.0,  1.0, -1.0,    0.0,
};

static uint16_t cube_indices[] = {
  0, 1, 2,     0, 2, 3,
  6, 5, 4,     7, 6, 4,
  8, 9, 10,    8, 10, 11,
  14, 13, 12,  15, 14, 12,
  16, 17, 18,  16, 18, 19,
  22, 21, 20,  23, 22, 20
};


BoxdrawRenderer boxdraw_create() {
  BoxdrawRenderer result = {};
  result.commands = (BoxdrawCommand*)calloc(BOXDRAW_CMD_MAX, sizeof(BoxdrawCommand));

  result.shader = sg_make_shader(samalg_shader_desc(sg_query_backend()));

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
  pipeline_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT;  // is bottom vertex
  pipeline_desc.colors[0].blend.enabled = true;
  pipeline_desc.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
  pipeline_desc.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
  pipeline_desc.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_ONE;
  pipeline_desc.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ZERO;
  pipeline_desc.shader = result.shader;
  pipeline_desc.index_type = SG_INDEXTYPE_UINT16;
  pipeline_desc.cull_mode = SG_CULLMODE_BACK;
  pipeline_desc.depth.write_enabled = true;
  pipeline_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
  pipeline_desc.label = "cube-pipeline";
  result.pipeline = sg_make_pipeline(pipeline_desc);

  result.bindings.vertex_buffers[0] = result.vertex_buffer;
  result.bindings.index_buffer = result.index_buffer;

  // initial clear color
  result.pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
  result.pass_action.colors[0].store_action = SG_STOREACTION_STORE;
  result.pass_action.colors[0].clear_value = { 0.0f, 0.0f, 0.5f, 1.0f };

  return result;
}

void boxdraw_destroy(BoxdrawRenderer *renderer) {
  sg_destroy_buffer(renderer->vertex_buffer);
  sg_destroy_buffer(renderer->index_buffer);
  sg_destroy_pipeline(renderer->pipeline);
  sg_destroy_shader(renderer->shader);
}

void boxdraw_push(BoxdrawRenderer *renderer, BoxdrawCommand cmd) {
  assert(renderer->commands_count < BOXDRAW_CMD_MAX);
  renderer->commands[renderer->commands_count++] = cmd;
}

/*
  * This function creates a transform for cube_vertices, so that it's positioned
 * at the correct corners of the specified box. 
 */
Matrix4 create_box_transform(Box3 box) {
  float width, height, depth;
  width  = box.max.x-box.min.x;
  height = box.max.y-box.min.y;
  depth  = box.max.z-box.min.z;

  Vector3 offset = box.min;
  
  /*
   * Uncenter the cube.
   */
  offset.x += width  / 2; 
  offset.y += height / 2;
  offset.z += depth  / 2;

  Matrix4 result = 
    Matrix4::translate(offset) *
    Matrix4::scale_v({width, height, depth}) * 
    Matrix4::scale(0.5) /* Scale by half, because cube_indices spans -1, 1 range (2 units) */
    ;
 
  return result;
}

void boxdraw_flush(BoxdrawRenderer *renderer, Matrix4 view_projection) {
  const int width = sapp_width();
  const int height = sapp_height();

  sg_begin_default_pass(&renderer->pass_action, width, height);
  sg_apply_pipeline(renderer->pipeline);
  sg_apply_bindings(renderer->bindings);

  for (size_t i = 0; i < renderer->commands_count; i++) {
    BoxdrawCommand command = renderer->commands[i];

    Matrix4 model = create_box_transform(command.box);

    samalg_vs_params_t vs_params;
    vs_params.mvp = view_projection * model;
    vs_params.top_color = command.top_color;
    vs_params.bottom_color = command.bottom_color;

    sg_range params_range = SG_RANGE(vs_params);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_samalg_vs_params, &params_range);
    sg_draw(0, 36, 1);
  }

  renderer->commands_count = 0;
  sg_end_pass();
}