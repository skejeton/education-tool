#pragma once
#include "../render_buffer.hpp"
#include "catedu/core/math/math.hpp"
#include <functional>

typedef std::function<Vector2(float)> Float2Point;

/**
 * @brief Generates a mesh centered around the origin.
 *
 * @param desc The description of the mesh.
 * @param sample_count The number of samples to take.
 * @param range_begin The beginning of the range to sample.
 * @param range_end The end of the range to sample.
 * @param function The function to sample.
 * @return The mesh.
 * @note The function should be defined on the range [range_begin, range_end].
 * @note The function only works for convex functions.
 */
RenderGeo rendering_2d_generate_mesh_centered_function(RenderWriteDesc desc,
                                                       size_t sample_count,
                                                       float range_begin,
                                                       float range_end,
                                                       Float2Point sampler);

RenderGeo rendering_2d_generate_squircle(RenderWriteDesc desc,
                                         size_t sample_count, float n,
                                         Vector2 radius);

RenderGeo rendering_2d_generate_circle(RenderWriteDesc desc,
                                       size_t sample_count);

RenderGeo rendering_2d_generate_square(RenderWriteDesc desc);
