#pragma once
#include "catedu/core/math/math.hpp"
#include "render_buffer.hpp"
#include <vector>


/// @brief A generator for render meshes.
struct RenderMeshGenerator
{
    std::vector<float> vertices;
    std::vector<uint16_t> indices;
    size_t vertex_stride;

    static RenderMeshGenerator init(size_t vertex_stride);

    uint16_t push_vert(float *vertex, size_t count);

    void push_tri(uint16_t a, uint16_t b, uint16_t c);

    /// TODO: maybe return the buffer with the vertex stride and
    ///       allow to merge buffers instead.
    RenderGeo convert(RenderWriteDesc desc);
};
