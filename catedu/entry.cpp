#include "entry.hpp"
#include "boxdraw.hpp"
#include "catedu/core/math/point_intersect.hpp"
#include "catedu/sys/sg_tricks.hpp"
#include "catedu/ui/rendering/make_brush.hpp"
#include "catedu/ui/resources/load_image.hpp"
#include "enet/enet.h"
#include "math.hpp"
#include <cstdlib>
#include <filesystem>

Texture tex;

void
Entry::frame(void)
{
    const float width = sapp_widthf();
    const float height = sapp_heightf();
    static float time = 0;
    time += sapp_frame_duration();

    Camera camera = Camera::init(45);
    camera.set_aspect(width / height);
    camera.move(0, 10, -10);
    camera.rotate(0, -45);
    camera.rotate_around({ 0, 0, 0 }, -time * 5, 0);

    for (int i = -4; i < 5; i++) {
        for (int j = -4; j < 5; j++) {
            boxdraw_push(&this->boxdraw_renderer,
                         boxdraw_cmdtexture(
                           box3_extrude_from_point({ float(i), 0, float(j) },
                                                   { 0.5f, 0.5f, 0.5f }),
                           tex.cropped({ 0, 32, 32, 32 })));
        }
    }
    char* s = "### ###\n"
              "#     #\n"
              "#     #\n"
              "#     #\n"
              "#     #\n"
              "#     #\n"
              "#######\n";

    {
        int x = 0, y = 0;
        for (int i = 0; s[i]; i++) {
            if (s[i] == '\n') {
                x = 0;
                y++;
                continue;
            } else if (s[i] == '#') {
                boxdraw_push(
                  &this->boxdraw_renderer,
                  boxdraw_cmdtexture(
                    box3_extrude_from_point({ float(x - 3), 1, float(y - 3) },
                                            { 0.5f, 0.5f, 0.5f }),
                    tex.cropped({ 64, 32, 32, 32 })));
                x++;
            } else {
                boxdraw_push(
                  &this->boxdraw_renderer,
                  boxdraw_cmdtexture(box3_extrude_from_point(
                                       { float(x - 3), 0.55, float(y - 3) },
                                       { 0.5f, 0.05f, 0.5f }),
                                     tex.cropped({ 32, 32, 32, 32 })));
                x++;
            }
        }
    }

    {
        int x = 0, y = 0;
        for (int i = 0; s[i]; i++) {
            if (s[i] == '\n') {
                x = 0;
                y++;
                continue;
            } else if (s[i] == '#') {
                boxdraw_push(
                  &this->boxdraw_renderer,
                  boxdraw_cmdtexture(
                    box3_extrude_from_point({ float(x - 3), 2, float(y - 3) },
                                            { 0.5f, 0.5f, 0.5f }),
                    tex.cropped({ 64, 32, 32, 32 })));
                x++;
            } else {
                x++;
            }
        }
    }

    {
        int x = 0, y = 0;
        for (int i = 0; s[i]; i++) {
            if (s[i] == '\n') {
                x = 0;
                y++;
                continue;
            } else if (s[i] == '#') {
                if ((x + y) % 2 == 0)
                    boxdraw_push(
                      &this->boxdraw_renderer,
                      boxdraw_cmdtexture(box3_extrude_from_point(
                                           { float(x - 3), 3, float(y - 3) },
                                           { 0.5f, 0.5f, 0.5f }),
                                         tex.cropped({ 64, 32, 32, 32 })));
                x++;
            } else {
                x++;
            }
        }
    }

    boxdraw_flush(&this->boxdraw_renderer, camera.vp);

    this->main_menu.show();

    sg_commit();
}

void
Entry::cleanup(void)
{
    tex.deinit();
    boxdraw_destroy(&this->boxdraw_renderer);
    sg_tricks_deinit();
    sg_shutdown();
    enet_deinitialize();
}

void
Entry::init()
{
    console_create_or_bind_existing();

    sg_desc desc = {};
    desc.context = sapp_sgcontext();
    desc.logger.func = slog_func;
    sg_setup(&desc);
    sg_tricks_init();
    tex = Texture::init("./assets/test_spritesheet_01.png");

    enet_initialize();

    main_menu = GuiMainMenu::init();
    this->boxdraw_renderer = boxdraw_create();
}

void
Entry::input(const sapp_event* event)
{
    main_menu.ui_state.feed_event(event);
}
