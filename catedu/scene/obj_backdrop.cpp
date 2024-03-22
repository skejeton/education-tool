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

void ObjBackdrop::update(PhysicsWorld &world, ResourceSpec &resources)
{
}

void ObjBackdrop::render(catedu::pbr::Renderer &renderer,
                         ResourceSpec &resources)
{
    // boxdraw_push(
    //     &renderer,
    //     boxdraw_cmdtexture(
    //         box3_extrude_from_point({32, -1, 32}, {64, 0.5, 64}),
    //         resources.tileset.cropped({0, 32, 32, 32}).tiled({128, 128})));
}

ObjBackdrop ObjBackdrop::copy()
{
    return *this;
}

ObjBackdrop ObjBackdrop::load(void **data_)
{
    uint8_t *data = (uint8_t *)*data_;
    *data_ = data + sizeof(Rect);
    return {*(Rect *)(data)};
}

void ObjBackdrop::save(BumpAllocator &alloc)
{
    *(Rect *)alloc.alloc(sizeof(this->texture_rect)) = this->texture_rect;
}
