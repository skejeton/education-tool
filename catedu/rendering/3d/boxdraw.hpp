///
/// BRIEF: Box drawing utilities (mostly for debugging)
///

#pragma once
#define BOXDRAW_CMD_MAX (1 << 14)

#include "catedu/core/math/math.hpp"
#include "catedu/rendering/3d/model.hpp"
#include "catedu/rendering/texture.hpp"
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"

struct BoxdrawCommand
{
    Box3 box;
    Vector4 top_color, bottom_color;
    Texture texture;
    bool transparent;
};

struct BoxdrawRenderer
{
    sg_pass_action pass_action;
    sg_buffer instance_buffer;
    sg_buffer vertex_buffer;
    sg_buffer index_buffer;
    sg_bindings bindings;
    sg_pipeline pipeline;
    sg_shader shader;
    size_t index_count;
    BoxdrawCommand *commands;
    size_t commands_count;
    catedu::Model model;
};

BoxdrawRenderer boxdraw_create();
void boxdraw_destroy(BoxdrawRenderer *renderer);
void boxdraw_push(BoxdrawRenderer *renderer, BoxdrawCommand cmd);
void boxdraw_flush(BoxdrawRenderer *renderer, Matrix4 view_projection);

inline BoxdrawCommand boxdraw_cmdcolored(Box3 box, Vector4 color,
                                         bool transparent = false)
{
    return {box, color, color, transparent};
}

inline BoxdrawCommand boxdraw_cmdgradient(Box3 box, Vector4 top_color,
                                          Vector4 bottom_color,
                                          bool transparent = false)
{
    return {box, top_color, bottom_color, transparent};
}

inline BoxdrawCommand boxdraw_cmdtexture(Box3 box, Texture texture,
                                         bool transparent = false)
{
    return {
        box, {1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, texture, transparent};
}
