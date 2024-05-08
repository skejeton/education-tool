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
    pbr_vs_params_t vs_params;

    vs_params.model =
        Matrix4::translate(renderer.camera.position) * Matrix4::scale(8);
    vs_params.lightness = 1.0f;
    vs_params.color_mul = {1.0f, 1.0f, 1.0f, 1.0f};
    renderer.render_model(
        resources.models.get_assert(resources.find_model_by_name("skybox"))
            .model,
        vs_params);

    vs_params.model = Matrix4::identity();
    vs_params.lightness = 0.0f;
    vs_params.color_mul = {1.0f, 1.0f, 1.0f, 1.0f};
    renderer.render_model(
        resources.models.get_assert(resources.find_model_by_name("grass_floor"))
            .model,
        vs_params);
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
