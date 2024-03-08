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
        popuptype = 3;
    }
    if (button(user, "Exit"))
    {
        popuptype = 1;
    }

    user.end_generic();
    user.end_generic();
    user.end_generic();

    if (popuptype == 1)
    {
        const char *buttons[] = {"Yes", "No", NULL};
        switch (msgbox(user, "Exit", "Are you sure you want to exit?",
                       MsgBoxType::Warning, buttons))
        {
        case 0:
            sapp_request_quit();
            break;
        case 1:
            popuptype = 0;
            break;
        }
    }
    else if (popuptype == 3)
    {
        const char *buttons[] = {"Close", "Scale+", "Scale-", NULL};
        switch (msgbox(user, "Settings", "Change the scale of the UI.",
                       MsgBoxType::Info, buttons))
        {
        case 0:
            popuptype = 0;
            break;
        case 1:
            scale_delta = 0.1;
            break;
        case 2:
            scale_delta = -0.1;
            break;
        }
    }
    else if (popuptype != 0)
    {
        const char *buttons[] = {"Ok", NULL};
        switch (msgbox(user, "Info", "This feature is not implemented yet.",
                       MsgBoxType::Error, buttons))
        {
        case 0:
            popuptype = 0;
        }
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
