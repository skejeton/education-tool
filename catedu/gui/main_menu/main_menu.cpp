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

AutoLayoutElement
make_element(AutoLayout layout, Vector2 size, bool autox, bool autoy)
{
    AutoLayoutElement el = {};
    el.layout = layout;
    el.width = { autox ? AutoLayoutDimension::Auto : AutoLayoutDimension::Pixel,
                 size.x };
    el.height = { autoy ? AutoLayoutDimension::Auto
                        : AutoLayoutDimension::Pixel,
                  size.y };
    el.padding = { 3, 3, 3, 3 };
    el.margin = { 3, 3, 3, 3 };
    el.border = { 3, 3, 3, 3 };

    return el;
}

void
GuiMainMenu::show()
{
    static std::string log = "";
    static float thickness = 1;

    auto user = UiUser::init(this->ui_state);
    user.begin_pass();

    user.begin_generic(make_element({ AutoLayout::Row }, { 0, 0 }, true, true),
                       UiMakeBrush::make_solid({ 1, 0, 0, 0.2 }),
                       UiMakeBrush::make_solid({ 0, 0, 0, 0.2 }));

    user.begin_generic(
      make_element({ AutoLayout::Column }, { 120, 40 }, false, false),
      UiMakeBrush::make_solid({ 1, 0, 0, 1 }),
      UiMakeBrush::make_solid({ 0, 0, 0, 1 }));

    user.end_generic();

    user.begin_generic(
      make_element({ AutoLayout::Row }, { 43, 52 }, false, false),
      UiMakeBrush::make_solid({ 1, 0, 0, 1 }),
      UiMakeBrush::make_solid({ 0, 0, 0, 1 }));

    user.end_generic();
    user.begin_generic(make_element({ AutoLayout::Row }, { 0, 0 }, true, true),
                       UiMakeBrush::make_solid({ 1, 0, 0, 1 }),
                       UiMakeBrush::make_solid({ 0, 0, 0, 1 }));

    user.begin_generic(
      make_element({ AutoLayout::Column }, { 120, 54 }, false, false),
      UiMakeBrush::make_solid({ 1, 0, 0, 1 }),
      UiMakeBrush::make_solid({ 0, 0, 0, 1 }));
    user.end_generic();
    user.begin_generic(
      make_element({ AutoLayout::Column }, { 10, 60 }, false, false),
      UiMakeBrush::make_solid({ 1, 0, 0, 1 }),
      UiMakeBrush::make_solid({ 0, 0, 0, 1 }));
    user.end_generic();

    user.end_generic();

    user.end_generic();

    user.end_pass();
}
