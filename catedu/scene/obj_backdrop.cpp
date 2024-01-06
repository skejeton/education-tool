#include "obj_backdrop.hpp"



ObjBackdrop ObjBackdrop::init(Rect texture_rect)
{
    ObjBackdrop result = {};
    result.texture_rect = texture_rect;
    return result;
}

void ObjBackdrop::deinit()
{

}

void ObjBackdrop::render(BoxdrawRenderer &renderer, ResourceSpec &resources)
{
    boxdraw_push(
        &renderer,
        boxdraw_cmdtexture(
            box3_extrude_from_point({32, -1, 32}, {32, 0.5, 32}),
            resources.tileset.cropped({0, 32, 32, 32}).tiled({64, 64})));
}
