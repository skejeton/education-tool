#include "catedu/ui/rendering/core.hpp"
#include "lib/sokol/sokol_gfx.h"

struct OffscreenTarget
{
    sg_attachments attachment;
    sg_image img;
    sg_image depth;
    sg_sampler sampler;
    UiImageId id;
};

void offscreen_init_targets();
sg_pass_action offscreen_pass_action();
void offscreen_clear();
sg_attachments offscreen_alloc(UiRenderingCore *core, UiImageId &id_out);
void offscreen_deinit_targets(UiRenderingCore *core);
