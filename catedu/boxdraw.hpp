///
/// BRIEF: Box drawing utilities (mostly for debugging)
///

#ifndef H_BOXDRAW_CATEDU
#define H_BOXDRAW_CATEDU
#define BOXDRAW_CMD_MAX (1 << 14)

#include "math.hpp"
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "staging/texture.hpp"

struct BoxdrawCommand
{
    Box3 box;
    Vector4 top_color, bottom_color;
    Texture texture;
};

struct BoxdrawRenderer
{
    sg_pass_action pass_action;
    sg_buffer vertex_buffer;
    sg_buffer index_buffer;
    sg_bindings bindings;
    sg_pipeline pipeline;
    sg_shader shader;
    BoxdrawCommand* commands;
    size_t commands_count;
};

BoxdrawRenderer
boxdraw_create();
void
boxdraw_destroy(BoxdrawRenderer* renderer);
void
boxdraw_push(BoxdrawRenderer* renderer, BoxdrawCommand cmd);
void
boxdraw_flush(BoxdrawRenderer* renderer, Matrix4 view_projection);

inline BoxdrawCommand
boxdraw_cmdcolored(Box3 box, Vector4 color)
{
    return { box, color, color };
}

inline BoxdrawCommand
boxdraw_cmdgradient(Box3 box, Vector4 top_color, Vector4 bottom_color)
{
    return { box, top_color, bottom_color };
}

inline BoxdrawCommand
boxdraw_cmdtexture(Box3 box, Texture texture)
{
    return { box, { 1.0, 1.0, 1.0, 1.0 }, { 1.0, 1.0, 1.0, 1.0 }, texture };
}

#endif // H_BOXDRAW_CATEDU
