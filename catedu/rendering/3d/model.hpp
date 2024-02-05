#pragma once

#include "cgltf/cgltf.h"
#include "sokol/sokol_gfx.h"

namespace catedu
{

struct RawModel
{
    cgltf_data *data;

    static bool load_gltf(const char *path, RawModel &dest);
};

void print_info(RawModel &model);

struct Model
{
    sg_buffer vertex_buffer;
    sg_buffer index_buffer;
    size_t index_count;

    static bool load_from_raw(RawModel &raw, Model &dest);
};

} // namespace catedu
