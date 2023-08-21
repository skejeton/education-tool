#ifndef HPP_CATEDU_RENDERING_2D_GENERATE_MESH
#define HPP_CATEDU_RENDERING_GENERATE_MESH

#include <functional>
#include "../render_buffer.hpp"
#include "catedu/math.hpp"

/// @note Only works for convex shapes.
RenderGeo rendering_2d_generate_mesh_centered_function(
    RenderWriteDesc desc,
    size_t sample_count,
    float range_begin,
    float range_end,
    std::function<Vector2(float)> function
);

RenderGeo rendering_2d_generate_squircle(
    RenderWriteDesc desc,
    size_t sample_count,
    float n,
    float a,
    float b
);

RenderGeo rendering_2d_generate_circle(
    RenderWriteDesc desc,
    size_t sample_count
);

RenderGeo rendering_2d_generate_square(
    RenderWriteDesc desc
);


#endif
