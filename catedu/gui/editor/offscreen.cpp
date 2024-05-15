#include "offscreen.hpp"

enum
{
    CACHE_SIZE = 8
};

OffscreenTarget cache[CACHE_SIZE] = {};
size_t available_target = 0;

void offscreen_init_targets(UiRenderingCore *core)
{
    printf("INIT: offscreen targets\n");

    for (size_t i = 0; i < CACHE_SIZE; i++)
    {
        OffscreenTarget target = {0};

        sg_image_desc img_desc = {};
        img_desc.render_target = true;
        img_desc.width = 128;
        img_desc.height = 128;
        img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
        img_desc.sample_count = 1;

        sg_sampler_desc sampler_desc = {};
        sampler_desc.min_filter = SG_FILTER_LINEAR;
        sampler_desc.mag_filter = SG_FILTER_LINEAR;
        sampler_desc.wrap_u = SG_WRAP_CLAMP_TO_BORDER;
        sampler_desc.wrap_v = SG_WRAP_CLAMP_TO_BORDER;

        target.img = sg_make_image(&img_desc);
        img_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
        target.depth = sg_make_image(&img_desc);
        target.sampler = sg_make_sampler(&sampler_desc);

        sg_attachments_desc att_desc = {};
        att_desc.colors[0].image = target.img;
        att_desc.depth_stencil.image = target.depth;
        target.attachment = sg_make_attachments(att_desc);

        UiImage img = {};
        img.sampler = target.sampler;
        img.image = target.img;
        img.size = {128, 128};

        target.id = core->alloc_image(img);

        cache[i] = target;
    }
}

sg_pass_action offscreen_pass_action()
{
    sg_pass_action result = {};
    result.colors[0].load_action = SG_LOADACTION_CLEAR;
    result.colors[0].store_action = SG_STOREACTION_STORE;
    result.colors[0].clear_value = {0.0f, 0.0f, 0.0f, 0.0f};
    return result;
}

void offscreen_clear()
{
    available_target = 0;
}

sg_attachments offscreen_alloc(UiImageId &id_out)
{
    if (available_target >= CACHE_SIZE)
    {
        fprintf(stderr, "ERROR: No more offscreen targets available\n");
        return {};
    }

    id_out = cache[available_target].id;

    return cache[available_target++].attachment;
}

void offscreen_deinit_targets(UiRenderingCore *core)
{
    printf("DEINIT: offscreen targets\n");

    for (size_t i = 0; i < CACHE_SIZE; i++)
    {
        sg_destroy_image(cache[i].depth);
        sg_destroy_attachments(cache[i].attachment);

        // The image and sampler is destroyed by the core
        core->dealloc_image(cache[i].id);
    }
}
