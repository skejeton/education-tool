#ifndef HPP_CATEDU_STAGING_TEXTURE
#define HPP_CATEDU_STAGING_TEXTURE

#include "catedu/math.hpp"
#include "sokol/sokol_gfx.h"

struct Texture
{
    sg_image sysid_texture;
    sg_sampler sysid_sampler;
    Vector2 size;

    bool if_crop;
    Rect crop;

    static Texture init(const char* png_path);
    void deinit();

    Texture uncropped();
    Texture cropped(Rect crop);

    Vector2 uv_min();
    Vector2 uv_max();

    bool if_valid();
};

#endif // HPP_CATEDU_STAGING_TEXTURE
