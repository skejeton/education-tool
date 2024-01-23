/// TODO: respect winding (currently its CCW if function goes clockwise)
#include "generate_mesh.hpp"
#include "../mesh_generator.hpp"
#include "catedu/core/math/interpolation.hpp"
#include "catedu/core/math/superellipse.hpp"

#define PUSH_2D_V values, 2

RenderGeo rendering_2d_generate_mesh_centered_function(
    RenderWriteDesc desc, size_t sample_count, float range_begin,
    float range_end, std::function<Vector2(float)> function)
{
    auto generator = RenderMeshGenerator::init(2);
    uint16_t center = generator.push_vert(Vector2{0, 0}.PUSH_2D_V);
    uint16_t previous = 0;
    uint16_t first = 0;

    for (size_t i = 0; i < sample_count; i++)
    {
        float t = math_lerp(range_begin, range_end, (float)i / sample_count);
        uint16_t current = generator.push_vert(function(t).PUSH_2D_V);
        if (previous)
        {
            generator.push_tri(center, current, previous);
        }
        else
        {
            first = current;
        }
        previous = current;
    }
    generator.push_tri(center, first, previous);

    return generator.convert(desc);
}

RenderGeo rendering_2d_generate_squircle(RenderWriteDesc desc,
                                         size_t sample_count, float n, float a,
                                         float b)
{
    auto generator = [n, a, b](float theta) {
        Polar2 polar = superellipse_generate(n, a, b, theta);
        return polar2_into_cartesian(polar);
    };

    return rendering_2d_generate_mesh_centered_function(desc, sample_count, 0,
                                                        MATH_TAU, generator);
}

RenderGeo rendering_2d_generate_circle(RenderWriteDesc desc,
                                       size_t sample_count)
{
    auto generator = [](float theta) {
        return Vector2{cosf(theta) * 0.5f, sinf(theta) * 0.5f};
    };

    return rendering_2d_generate_mesh_centered_function(desc, sample_count, 0,
                                                        MATH_TAU, generator);
}

RenderGeo rendering_2d_generate_square(RenderWriteDesc desc)
{
    auto generator = RenderMeshGenerator::init(2);
    uint16_t a = generator.push_vert(Vector2{-0.5, 0.5}.PUSH_2D_V);
    uint16_t b = generator.push_vert(Vector2{0.5, 0.5}.PUSH_2D_V);
    uint16_t c = generator.push_vert(Vector2{0.5, -0.5}.PUSH_2D_V);
    uint16_t d = generator.push_vert(Vector2{-0.5, -0.5}.PUSH_2D_V);

    generator.push_tri(a, b, c);
    generator.push_tri(a, c, d);

    return generator.convert(desc);
}
