#ifndef HPP_CATEDU_RENDERING_RENDER_BUFFER
#define HPP_CATEDU_RENDERING_RENDER_BUFFER

#include <stddef.h>
#include <stdint.h>

/// @brief Describes a write to a render buffer.
struct RenderWriteDesc {
    /// @brief In float element count.
    size_t stride;
    /// @brief In float element count.
    size_t offset;
};

/// @brief Describes a render buffer.
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

/**
 * @brief Populates a render buffer with the same data.
 *
 * @param geo The render buffer to populate.
 * @param desc The description of the write.
 * @param data The data to write.
 * @param count The number of elements to write.
 */
void render_populate_all(RenderGeo *geo, RenderWriteDesc desc, float *data, size_t data_size);

#endif
