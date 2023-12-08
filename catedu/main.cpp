// TODO: Use an allocator instead.

#include "entry.hpp"

#pragma region initialize
static Entry *userdata_to_entry(void *userdata)
{
    return (Entry *)userdata;
}

static void init_callback(void *userdata)
{
    userdata_to_entry(userdata)->init();
}

static void frame_callback(void *userdata)
{
    userdata_to_entry(userdata)->frame();
}

static void cleanup_callback(void *userdata)
{
    userdata_to_entry(userdata)->cleanup();
    delete (userdata_to_entry(userdata));
}

static void event_callback(const sapp_event *event, void *userdata)
{
    userdata_to_entry(userdata)->input(event);
}

static sapp_desc create_app_description()
{
    // Wish C++ had using for variables...
    sapp_desc r = {};

    // Instead of using a global variable
    // FIXME: I have to use `new` here because C++ is garbage and if I use
    //        calloc, the constructor of some sub-fields of Entry will not be
    //        called, which will crash during move because it will attempt to
    //        destruct some uninitialized STL structures so I have to us `new`.
    //        -_-
    r.user_data = (void *)new Entry();

    // Userdata callbacks
    r.init_userdata_cb = init_callback;
    r.frame_userdata_cb = frame_callback;
    r.cleanup_userdata_cb = cleanup_callback;
    r.event_userdata_cb = event_callback;

    // Window settings
    r.window_title = "Education tool";
    r.high_dpi = true;
    r.width = 1024;
    r.height = 768;
    r.sample_count = 1;
    r.icon.sokol_default = false;

    // System settings
    r.enable_clipboard = true;

    // Logging settings
    r.logger.func = slog_func;

    return r;
}
#pragma endregion

#pragma region main
sapp_desc sokol_main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    return create_app_description();
}
#pragma endregion
