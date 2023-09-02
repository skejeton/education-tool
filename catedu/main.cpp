#include "entry.hpp"

Entry global_entry = {};

sapp_desc
sokol_main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    sapp_desc desc = {};
    desc.init_cb = []() { global_entry.init(); };
    desc.frame_cb = []() { global_entry.frame(); };
    desc.cleanup_cb = []() { global_entry.cleanup(); };
    desc.event_cb = [](auto event) { global_entry.input(event); };
    desc.width = 1024;
    desc.height = 768;
    desc.high_dpi = true;
    desc.window_title = "Education tool";
    desc.ios_keyboard_resizes_canvas = false;
    desc.icon.sokol_default = true;
    desc.enable_clipboard = true;
    desc.logger.func = slog_func;
    desc.sample_count = 4;
    return desc;
}
