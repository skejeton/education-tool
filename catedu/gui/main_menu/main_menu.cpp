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

    user.begin_generic({ 64, 128 },
                       UiMakeBrush::make_solid({ 1, 0, 0, 1 }),
                       4,
                       UiMakeBrush::make_solid({ 0, 0, 0, 1 }));

    user.end_generic();

    user.begin_generic({ 400, 64 },
                       UiMakeBrush::make_solid({ 1, 0, 0, 1 }),
                       1,
                       UiMakeBrush::make_solid({ 0, 0, 0, 1 }));

    user.end_generic();

    user.begin_generic({ 54, 54 },
                       UiMakeBrush::make_solid({ 1, 0, 0, 1 }),
                       1,
                       UiMakeBrush::make_solid({ 0, 0, 0, 1 }));

    user.begin_generic({ 120, 54 },
                       UiMakeBrush::make_solid({ 1, 0, 0, 1 }),
                       1,
                       UiMakeBrush::make_solid({ 0, 0, 0, 1 }));
    user.end_generic();
    user.begin_generic({ 10, 60 },
                       UiMakeBrush::make_solid({ 1, 0, 0, 1 }),
                       1,
                       UiMakeBrush::make_solid({ 0, 0, 0, 1 }));
    user.end_generic();

    user.end_generic();

    user.end_pass();
}
