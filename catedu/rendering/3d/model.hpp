#pragma once

#include "catedu/rendering/texture.hpp"
#include "cgltf/cgltf.h"
#include "sokol/sokol_gfx.h"

struct RawModel
{
    cgltf_data *data;
    char *texture_path;

    static bool load_gltf(const char *path, RawModel &dest);
    // Submodels share the geometry but not the texture: submodel N of a model
    // textured with "cube.png" uses "cube_N.png".
    bool submodel_texture_path(int submodel, char *dest, size_t size) const;
    void deinit();
};

void print_info(RawModel &model);

struct Model
{
    sg_buffer vertex_buffer;
    sg_buffer index_buffer;
    Texture texture;
    size_t index_count;

    static bool load_from_raw(RawModel &raw, Model &dest, int submodel = 0);
    void deinit();
};
