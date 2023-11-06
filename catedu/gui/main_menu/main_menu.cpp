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
make_element(AutoLayout layout,
             Vector2 size,
             bool autox,
             bool autoy,
             Vector2 align = { 0, 0 })
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
    el.align_width = align.x;
    el.align_height = align.y;

    return el;
}

AutoLayoutElement
make_auto(AutoLayout layout)
{
    AutoLayoutElement el = {};
    el.layout = layout;
    return el;
}

void
GuiMainMenu::show()
{
    static bool exitpopup = false;
    static std::string logs = "\n";
    auto user = UiUser::init(this->ui_state);
    user.begin_pass();

    user.begin_generic(
      make_element(
        { AutoLayout::Column }, { 0, sapp_heightf() }, true, false, { 0, 0.5 }),
      UiMakeBrush::make_solid({ 0, 0, 0, 0.0 }),
      UiMakeBrush::make_solid({ 0, 0, 0, 0.0 }));

    user.begin_generic(make_auto({ AutoLayout::Column }), {}, {});
    user.label("Educat!", { 5, 5 }, UiMakeBrush::make_solid({ 1, 1, 1, 1 }));

    if (user.button("Play")) {
        logs += "Clicked play!\n";
    }
    if (user.button("Editor")) {
        logs += "Clicked editor!\n";
    }
    if (user.button("Settings")) {
        logs += "Clicked settings!\n";
    }
    if (user.button("Exit")) {
        exitpopup = true;
        logs += "Clicked exit!\n";
    }

    user.end_generic();
    user.end_generic();

    user.begin_generic(
      make_element({ AutoLayout::Column }, { 0, 0 }, true, true),
      UiMakeBrush::make_solid({ 1, 0, 0, 0.2 }),
      UiMakeBrush::make_solid({ 0, 0, 0, 0.2 }));

    user.begin_generic(
      make_element({ AutoLayout::Column }, { 0, 0 }, true, true),
      UiMakeBrush::make_solid({ 1, 0, 0.5, 1 }),
      UiMakeBrush::make_solid({ 0, 0, 0, 1 }));

    if (exitpopup) {
        user.begin_generic(
          make_element({ AutoLayout::Column }, { 0, 0 }, true, true),
          UiMakeBrush::make_solid({ 1, 0, 0.5, 1 }),
          UiMakeBrush::make_solid({ 0, 0, 0, 1 }));

        if (user.button("x")) {
            exitpopup = false;
        }
        user.label("Are you sure you want to exit?", { 2, 2 });
        user.begin_generic(make_auto({ AutoLayout::Row }), {}, {});
        if (user.button("Yes")) {
            sapp_request_quit();
        }
        if (user.button("No")) {
            exitpopup = false;
        }
        user.end_generic();
        user.end_generic();
    }

    user.begin_generic(
      make_element({ AutoLayout::Column }, { 10, 60 }, false, false),
      UiMakeBrush::make_solid({ 1, 0, 0.1, 1 }),
      UiMakeBrush::make_solid({ 0, 0, 0, 1 }));
    user.end_generic();

    user.label("Example 1: Rich text", { 3, 3 });
    user.label("This is a test label\nIt has multiline support!", { 2, 2 });
    user.label("Oooh look fancy!",
               { 4, 4 },
               UiMakeBrush::make_gradient({ 1, 0, 0, 1 }, { 0, 0, 1, 1 }));

    user.begin_generic(make_auto({ AutoLayout::Row }), {}, {});
    if (user.button("Click me!")) {
        logs += "Clicked button!\n";
    }
    if (user.button("Option 2.")) {
        logs += "Clicked option 2!\n";
    }
    /*
    if (user.button("Option 3.")) {
        logs += "Clicked option 3!\n";
    }
    if (user.button("Option 4!")) {
        logs += "Clicked option 4!\n";
    }
    */

    user.end_generic();

    user.end_generic();

    user.end_generic();

    user.begin_generic(
      make_element({ AutoLayout::Column }, { 0, 0 }, true, true),
      UiMakeBrush::make_solid({ 0.3, 0.3, 0.3, 1.0 }),
      UiMakeBrush::make_solid(UI_COLOR_BLACK));
    user.label("Example 2: Log list", { 3, 3 });
    user.label("Use the buttons on the right side to add new logs.",
               { 1.5, 1.5 },
               UiMakeBrush::make_solid({ 0.7, 0.7, 0.7, 1 }));

    user.label(logs.c_str());
    user.end_generic();

    user.end_pass();
}
