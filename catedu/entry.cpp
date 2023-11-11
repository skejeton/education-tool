#include "entry.hpp"
#include "boxdraw.hpp"
#include "catedu/core/math/point_intersect.hpp"
#include "catedu/ui/rendering/make_brush.hpp"
#include "catedu/ui/resources/load_image.hpp"
#include "enet/enet.h"
#include "math.hpp"
#include <cstdlib>
#include <filesystem>

void
Entry::frame(void)
{
    // this->main_menu.show();
    const float width = sapp_widthf();
    const float height = sapp_heightf();

    Camera camera = Camera::init(45);
    camera.set_aspect(width / height);
    camera.move(0, 20, -20);
    camera.rotate(0, -45);

    boxdraw_push(
      &this->boxdraw_renderer,
      boxdraw_cmdgradient(box3_extrude_from_point({ 0, 0, 0 }, { 4, 1, 4 }),
                          { 0.0, 1.0, 0.0, 1.0 },
                          { 1.0, 1.0, 0.0, 1.0 }));

    boxdraw_flush(&this->boxdraw_renderer, camera.vp);

    sg_commit();
}

void
Entry::cleanup(void)
{
    boxdraw_destroy(&this->boxdraw_renderer);
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

    enet_initialize();

    main_menu = GuiMainMenu::init();
    this->boxdraw_renderer = boxdraw_create();
}

void
Entry::input(const sapp_event* event)
{
    main_menu.ui_state.feed_event(event);
}
