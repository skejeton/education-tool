#include "render.hpp"

static Matrix4 box_to_matrix(Box3 box)
{
    Vector3 min = box.min;
    Vector3 max = box.max;

    return Matrix4::translate((min + max) * 0.5f) * Matrix4::scale_v(max - min);
}

static void render_component(catedu::pbr::Renderer &renderer,
                             const GenResources &resources,
                             const GeneratedObjectComponent &component)
{
    pbr_vs_params_t vs_params = {};
    vs_params.color_mul = component.color.to_vector4();
    vs_params.model = box_to_matrix(component.box);
    vs_params.lightness = 0;
    renderer.render_model(resources.box, vs_params);
}

void genobj_render_object(catedu::pbr::Renderer &renderer,
                          const GenResources &resources,
                          const GeneratedObject &object)
{
    for (size_t i = 0; i < object.num_components; i++)
    {
        render_component(renderer, resources, object.components[i]);
    }
}
