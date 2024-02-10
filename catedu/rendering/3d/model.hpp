#pragma once

#include "catedu/rendering/texture.hpp"
#include "cgltf/cgltf.h"
#include "sokol/sokol_gfx.h"

namespace catedu
{

struct RawModel
{
    cgltf_data *data;
    char *texture_path;

    static bool load_gltf(const char *path, RawModel &dest);
    void deinit();
};

void print_info(RawModel &model);

struct Model
{
    sg_buffer vertex_buffer;
    sg_buffer index_buffer;
    Texture texture;
    size_t index_count;

    static bool load_from_raw(RawModel &raw, Model &dest);
    void deinit();
};

} // namespace catedu
