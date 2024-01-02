#pragma once
#include "catedu/math.hpp"
#include "sokol/sokol_gfx.h"

struct Texture
{
    sg_image sysid_texture;
    sg_sampler sysid_sampler;
    Vector2 size;

    bool if_crop;
    Rect crop;
    Vector2 tile_count;

    static Texture init(const char *png_path);
    void deinit();

    Texture uncropped();
    Texture cropped(Rect crop);
    Texture tiled(Vector2 tile_count);

    Vector2 uv_min();
    Vector2 uv_max();

    bool if_valid();
};
