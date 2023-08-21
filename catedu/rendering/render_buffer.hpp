#ifndef HPP_CATEDU_RENDERING_RENDER_BUFFER
#define HPP_CATEDU_RENDERING_RENDER_BUFFER

#include <stddef.h>
#include <stdint.h>

struct RenderWriteDesc {
    /// @brief In float element count.
    size_t stride;
    /// @brief In float element count.
    size_t offset;
};

struct RenderGeo {
    float *vbuf;
    /// @brief In float element count.
    size_t vbuf_size;

    uint16_t *ibuf;
    /// @brief In u16 element count.
    size_t ibuf_size;

    /// @brief In float element count.
    size_t stride;

    bool validate();
};

void render_populate_all(RenderGeo *geo, RenderWriteDesc desc, float *data, size_t data_size);

#endif
