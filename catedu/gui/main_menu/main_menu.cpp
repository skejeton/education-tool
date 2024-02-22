#include "main_menu.hpp"
#include "catedu/misc/util.hpp"
#include "catedu/ui/rendering/make_brush.hpp"
#include "catedu/ui/widgets.hpp"
#include "sokol/sokol_app.h"

AutoLayoutElement make_element(AutoLayout layout, Vector2 size, bool autox,
                               bool autoy, Vector2 align = {0, 0}, float p = 3);

AutoLayoutElement make_auto(AutoLayout layout, Vector2 align = {0, 0});

GuiMainMenu GuiMainMenu::init(UiState *ui_state)
{
    return {ui_state};
}

void GuiMainMenu::deinit()
{
    // Nothing yet
}

int GuiMainMenu::show()
{
    float scale_delta = 0;
    int exitcode = 1;
    static int popuptype = 0;
    auto user = UiUser::init(*this->ui_state);
    user.begin_pass();

    user.begin_generic(
        make_element({AutoLayout::Row},
                     {sapp_widthf() / this->ui_state->dpi_scale,
                      sapp_heightf() / this->ui_state->dpi_scale},
                     false, false, {1, 0.5}, 0),
        UiMakeBrush::make_solid({1, 1, 1, 0.0}),
        UiMakeBrush::make_solid({1, 1, 0.5, 0.0}));

    user.begin_generic(make_auto({AutoLayout::Row}), {}, {});
    user.begin_generic(make_auto({AutoLayout::Column}, {1.0, 0}), {}, {});
    // Pretty colorful gradient brush
    UiBrush brush =
        UiMakeBrush::make_gradient({0.2, 0.8, 0.0, 1.0}, {0.5, 1.0, 0.0, 1.0});

    AutoLayoutElement el = make_element({AutoLayout::Row}, {0, 0}, true, true);
    user.begin_generic(el, {}, {});

    user.bold = true;
    label(user, "Edu", {2.5, 2.5}, brush);
    user.bold = false;
    label(user, "core", {2.5, 2.5}, brush);

    user.end_generic();

    // FIXME: Yes, I added paddings to the buttons using spaces.
    if (!settings)
    {
        if (button(user, "Play"))
        {
            popuptype = 2;
        }
        if (button(user, "Editor"))
        {
            exitcode = 2;
        }
        if (button(user, "Settings"))
        {
            settings = true;
        }
        if (button(user, "Exit"))
        {
            popuptype = 1;
        }
    }
    else
    {
        if (button(user, "Back"))
        {
            settings = false;
        }
        if (button(user, "Scale+"))
        {
            scale_delta += 0.1;
        }
        if (button(user, "Scale-"))
        {
            scale_delta -= 0.1;
        }
    }

    user.end_generic();
    user.end_generic();
    user.end_generic();

    if (popuptype)
    {
        begin_show_window(
            user, {"Alert", rect_center_rect(
                                sapp_screen_rect_scaled(ui_state->dpi_scale),
                                {0, 0, 350, 75})});
        user.begin_generic(make_auto({AutoLayout::Column}, {0, 0}), {}, {});

        user.begin_generic(make_auto({AutoLayout::Row}), {}, {});

        if (popuptype == 1)
        {
            user.begin_generic(make_element({AutoLayout::Column}, {32, 32},
                                            false, false, {0.5, 0.5}, 3),
                               UiMakeBrush::make_gradient({0.0, 0.0, 0.7, 1.0},
                                                          {0, 0.2, 1.0, 1.0}),
                               UiMakeBrush::make_solid({1, 1, 1, 1.0}));
            label(user, "?", {2, 2}, UiMakeBrush::make_solid({1, 1, 1, 1.0}));
            user.end_generic();
        }
        else
        {
            user.begin_generic(make_element({AutoLayout::Column}, {32, 32},
                                            false, false, {0.5, 0.5}, 3),
                               UiMakeBrush::make_gradient({0.8, 0.5, 0.0, 1.0},
                                                          {1.0, 0.7, 0.0, 1.0}),
                               UiMakeBrush::make_solid({1, 1, 1, 1.0}));
            label(user, ":/", {2, 2}, UiMakeBrush::make_solid({1, 1, 1, 1.0}));
            user.end_generic();
        }

        if (popuptype == 1)
        {
            label(user, "\nAre you sure you want to exit?\n", {1, 1},
                  UiMakeBrush::make_solid({0, 0, 0, 1.0}));
        }
        else
        {
            label(user, "\nThis feature is not implemented yet.\n", {1, 1},
                  UiMakeBrush::make_solid({0, 0, 0, 1.0}));
        }

        user.end_generic();

        user.begin_generic(make_auto({AutoLayout::Row}), {}, {});
        if (popuptype == 1)
        {
            if (button(user, "Yes"))
            {
                popuptype = 0;
                sapp_request_quit();
            }
            if (button(user, "No"))
            {
                popuptype = 0;
            }
        }
        else
        {
            if (button(user, "Ok"))
            {
                popuptype = 0;
            }
        }

        user.end_generic();
        user.end_generic();
        end_show_window(user);
    }
    user.end_pass();

    user.state->dpi_scale += scale_delta;

    return exitcode;
}

AutoLayoutElement make_element(AutoLayout layout, Vector2 size, bool autox,
                               bool autoy, Vector2 align, float p)
{
    AutoLayoutElement el = {};
    el.layout = layout;
    el.width = {autox ? AutoLayoutDimension::Auto : AutoLayoutDimension::Pixel,
                size.x};
    el.height = {autoy ? AutoLayoutDimension::Auto : AutoLayoutDimension::Pixel,
                 size.y};
    el.padding = {p, p, p, p};
    el.margin = {p, p, p, p};
    el.border = {p, p, p, p};
    el.align_width = align.x;
    el.align_height = align.y;

    return el;
}

AutoLayoutElement make_auto(AutoLayout layout, Vector2 align)
{
    AutoLayoutElement el = {};
    el.layout = layout;
    el.align_width = align.x;
    el.align_height = align.y;
    return el;
}
