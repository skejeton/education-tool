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
             Vector2 align = { 0, 0 },
             float p = 3)
{
    AutoLayoutElement el = {};
    el.layout = layout;
    el.width = { autox ? AutoLayoutDimension::Auto : AutoLayoutDimension::Pixel,
                 size.x };
    el.height = { autoy ? AutoLayoutDimension::Auto
                        : AutoLayoutDimension::Pixel,
                  size.y };
    el.padding = { p, p, p, p };
    el.margin = { p, p, p, p };
    el.border = { p, p, p, p };
    el.align_width = align.x;
    el.align_height = align.y;

    return el;
}

AutoLayoutElement
make_auto(AutoLayout layout, Vector2 align = { 0, 0 })
{
    AutoLayoutElement el = {};
    el.layout = layout;
    el.align_width = align.x;
    el.align_height = align.y;
    return el;
}

void
GuiMainMenu::show()
{
    static int popuptype = 0;
    auto user = UiUser::init(this->ui_state);
    user.begin_pass();

    user.begin_generic(make_element({ AutoLayout::Row },
                                    { sapp_widthf(), sapp_heightf() },
                                    false,
                                    false,
                                    { 1, 0.5 },
                                    0),
                       UiMakeBrush::make_solid({ 1, 1, 1, 0.0 }),
                       UiMakeBrush::make_solid({ 1, 1, 0.5, 0.0 }));

    user.begin_generic(make_auto({ AutoLayout::Row }), {}, {});
    if (popuptype) {
        user.begin_generic(
          make_element(
            { AutoLayout::Column }, { 520, 250 }, false, true, { 0.0, 0.0 }, 1),
          UiMakeBrush::make_solid({ 0.7, 0.7, 0.7, 1.0 }),
          UiMakeBrush::make_solid({ 0, 0, 0, 1.0 }));

        if (user.button("x")) {
            popuptype = 0;
        }
        user.begin_generic(make_auto({ AutoLayout::Row }), {}, {});

        if (popuptype == 1) {
            user.begin_generic(make_element({ AutoLayout::Column },
                                            { 64, 64 },
                                            false,
                                            false,
                                            { 0.5, 0.5 },
                                            3),
                               UiMakeBrush::make_solid({ 0.0, 0.0, 0.7, 1.0 }),
                               UiMakeBrush::make_solid({ 1, 1, 1, 1.0 }));
            user.label(
              "?", { 5, 5 }, UiMakeBrush::make_solid({ 1, 1, 1, 1.0 }));
            user.end_generic();
        } else {
            user.begin_generic(make_element({ AutoLayout::Column },
                                            { 64, 64 },
                                            false,
                                            false,
                                            { 0.5, 0.5 },
                                            3),
                               UiMakeBrush::make_solid({ 1.0, 0.7, 0.0, 1.0 }),
                               UiMakeBrush::make_solid({ 1, 1, 1, 1.0 }));
            user.label(
              ":/", { 5, 5 }, UiMakeBrush::make_solid({ 1, 1, 1, 1.0 }));
            user.end_generic();
        }

        if (popuptype == 1) {
            user.label("\nAre you sure you want to exit?\n",
                       { 2, 2 },
                       UiMakeBrush::make_solid({ 0, 0, 0, 1.0 }));
        } else {
            user.label("\nThis feature is not implemented yet.\n",
                       { 2, 2 },
                       UiMakeBrush::make_solid({ 0, 0, 0, 1.0 }));
        }

        user.end_generic();
        user.begin_generic(make_auto({ AutoLayout::Row }), {}, {});
        if (popuptype == 1) {
            if (user.button("Yes")) {
                popuptype = 0;
                sapp_request_quit();
            }
            if (user.button("No")) {
                popuptype = 0;
            }
        } else {
            if (user.button("Ok")) {
                popuptype = 0;
            }
        }
        user.end_generic();

        user.end_generic();
    }
    user.begin_generic(make_auto({ AutoLayout::Column }, { 1.0, 0 }), {}, {});
    // Pretty colorful gradient brush
    UiBrush brush = UiMakeBrush::make_gradient({ 0.2, 0.8, 0.0, 1.0 },
                                               { 0.5, 1.0, 0.0, 1.0 });

    user.label("Educat!", { 5, 5 }, brush);

    // FIXME: Yes, I added paddings to the buttons using spaces.
    if (user.button("        Play        ")) {
        popuptype = 2;
    }
    if (user.button("        Editor        ")) {
        popuptype = 2;
    }
    if (user.button("        Settings        ")) {
        popuptype = 2;
    }
    if (user.button("        Exit        ")) {
        popuptype = 1;
    }

    user.end_generic();
    user.end_generic();
    user.end_generic();
    user.end_pass();
}
