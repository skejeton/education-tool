#include "entry.hpp"
#include "boxdraw.hpp"
#include "catedu/core/math/point_intersect.hpp"
#include "catedu/ui/rendering/make_brush.hpp"
#include "catedu/ui/resources/load_image.hpp"
#include "enet/enet.h"
#include "imgui/imgui.h"
#include "math.hpp"
#include <cstdlib>
#include <filesystem>

static void
set_imgui_rounding(float rounding)
{
    ImGui::GetStyle().TabRounding = rounding;
    ImGui::GetStyle().ChildRounding = rounding;
    ImGui::GetStyle().FrameRounding = rounding;
    ImGui::GetStyle().GrabRounding = rounding - 1;
    ImGui::GetStyle().WindowRounding = rounding;
    ImGui::GetStyle().ScrollbarRounding = rounding;
    ImGui::GetStyle().PopupRounding = rounding;
}

void
Entry::init(void)
{
    console_create_or_bind_existing();

    // setup sokol-gfx, sokol-time and sokol-imgui
    sg_desc desc = {};
    desc.context = sapp_sgcontext();
    desc.logger.func = slog_func;
    sg_setup(&desc);

    this->main_menu_b = GuiMainMenu::init();

    // use sokol-imgui with all default-options (we're not doing
    // multi-sampled rendering or using non-default pixel formats)
    simgui_desc_t simgui_desc = {};
    simgui_setup(&simgui_desc);

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
    simgui_shutdown();
    sg_shutdown();
    enet_deinitialize();
}

void
Entry::input(const sapp_event* event)
{
    if (pass_event(this->main_menu_b, event)) {
        return;
    }
}
