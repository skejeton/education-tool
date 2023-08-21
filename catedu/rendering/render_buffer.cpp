#include "render_buffer.hpp"
#include <string.h>

bool RenderGeo::validate()
{
    return this->vbuf_size % this->stride == 0
        && this->ibuf_size % 3 == 0;
}

void render_populate_all(RenderGeo *geo, RenderWriteDesc desc, float *data, size_t count)
{
    for (size_t i = 0; i < geo->vbuf_size; i += desc.stride) {
        memcpy(geo->vbuf + i + desc.offset, data, sizeof(float)*count);
    }
}
