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
Entry::init(void)
{
    console_create_or_bind_existing();

    sg_desc desc = {};
    desc.context = sapp_sgcontext();
    desc.logger.func = slog_func;
    sg_setup(&desc);

    this->main_menu_b = GuiMainMenu::init();

    enet_initialize();
}

void
Entry::frame(void)
{
    this->main_menu_b.show();

    sg_commit();
    inputs.update();
}

void
Entry::cleanup(void)
{
    sg_shutdown();
    enet_deinitialize();
}

void
Entry::input(const sapp_event* event)
{
}
