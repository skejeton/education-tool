#include "main_menu.hpp"
#include "catedu/core/math/interpolation.hpp"
#include "catedu/core/math/point_intersect.hpp"
#include "catedu/ui/rendering/colors.hpp"
#include "catedu/ui/rendering/make_brush.hpp"
#include "catedu/ui/rendering/pass.hpp"
#include "sokol/sokol_app.h"
#include <array>

GuiMainMenu
GuiMainMenu::init()
{
    return { UiState::init("./assets/Roboto-Regular.ttf") };
}

void
GuiMainMenu::show()
{
    static std::string log = "";
    static float thickness = 1;

    auto user = UiUser::init(this->ui_state);
    user.begin_pass();

    const char* buttons[] = { "Playtest", "Editor", "Settings", "Exit" };
    for (int i = 0; i < sizeof buttons / sizeof buttons[0]; i++) {
        if (user.button(buttons[i])) {
            log += std::string("Pressed ") + buttons[i] + "\n";
        }
    }

    user.generic({ 64, 64 },
                 UiMakeBrush::make_gradient(UI_COLOR_GREEN, UI_COLOR_MAGENTA),
                 thickness,
                 UiMakeBrush::make_solid(UI_COLOR_BLACK));

    if (user.button("Thicker")) {
        thickness += 1;
    }
    if (user.button("Thinner")) {
        thickness -= 1;
        if (thickness < 0)
            thickness = 0;
    }

    if (user.button("Clear")) {
        log = "";
    }

    user.label(log.c_str());

    user.end_pass();
}
