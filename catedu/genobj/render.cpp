#include "render.hpp"

static Matrix4 box_to_matrix(Box3 box)
{
    Vector3 min = box.min;
    Vector3 max = box.max;

    return Matrix4::translate((min + max) * 0.5f) * Matrix4::scale_v(max - min);
}

static void render_component(Renderer &renderer, const GenResources &resources,
                             const GeneratedObjectComponent &component,
                             Matrix4 matrix)
{
    Params vs_params = {};
    vs_params.color_mul = component.color.to_vector4();
    vs_params.model = matrix * box_to_matrix(component.box);
    vs_params.lightness = 0;
    if (component.shaded)
    {
        renderer.render_model(resources.box_shaded, vs_params);
    }
    else
    {
        renderer.render_model(resources.box, vs_params);
    }
}

void genobj_render_object(Renderer &renderer, const GenResources &resources,
                          const GeneratedObject &object, Matrix4 matrix)
{
    for (size_t i = 0; i < object.num_components; i++)
    {
        render_component(renderer, resources, object.components[i], matrix);
    }
}
