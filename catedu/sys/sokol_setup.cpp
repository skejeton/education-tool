#include "sokol_setup.hpp"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_glue.h"

static inline SokolSetup *userdata_to_setup(void *userdata)
{
    return (SokolSetup *)userdata;
}

static void init_callback(void *userdata)
{
    sg_desc desc = {};
    desc.context = sapp_sgcontext();
    desc.logger.func = slog_func;
    sg_setup(&desc);

    userdata_to_setup(userdata)->init();
}

static void frame_callback(void *userdata)
{
    userdata_to_setup(userdata)->frame();
}

static void cleanup_callback(void *userdata)
{
    userdata_to_setup(userdata)->cleanup();
    // FIXME: I have to use `delete` here because C++ is garbage and if I use
    //        calloc, the constructor of some sub-fields of userdata will not be
    //        called, which will crash during move because it will attempt to
    //        destruct some uninitialized STL structures so I have to use `new`.
    //        -_-
    delete (userdata_to_setup(userdata));
}

static void event_callback(const sapp_event *event, void *userdata)
{
    userdata_to_setup(userdata)->input(event);
}

sapp_desc sokol_setup_default(const char *title, SokolSetup *setup)
{
    // Wish C++ had `using` for variables...
    sapp_desc r = {};

    // Instead of using a global variable
    r.user_data = setup;

    // Userdata callbacks
    r.init_userdata_cb = init_callback;
    r.frame_userdata_cb = frame_callback;
    r.cleanup_userdata_cb = cleanup_callback;
    r.event_userdata_cb = event_callback;

    // Window settings
    r.window_title = title;
    r.high_dpi = true;
    r.width = 1024 * sapp_dpi_scale();
    r.height = 768 * sapp_dpi_scale();
    r.sample_count = 1;
    r.icon.sokol_default = false;

    // System settings
    r.enable_clipboard = true;

    // Logging settingsgg
    r.logger.func = slog_func;

    return r;
}
