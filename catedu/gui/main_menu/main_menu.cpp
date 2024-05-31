#include "main_menu.hpp"
#include "catedu/gui/transition/transition.hpp"
#include "catedu/ui/rendering/make_brush.hpp"
#include "catedu/ui/widgets.hpp"
#include "sokol/sokol_app.h"

AutoLayoutElement make_element(AutoLayout layout, Vector2 size, bool autox,
                               bool autoy, Vector2 align, float p = 3,
                               float m = 0)
{
    AutoLayoutElement el = {};
    el.layout = layout;
    el.width = {autox ? AutoLayoutDimension::Auto : AutoLayoutDimension::Pixel,
                size.x};
    el.height = {autoy ? AutoLayoutDimension::Auto : AutoLayoutDimension::Pixel,
                 size.y};
    el.padding = {p, p, p, p};
    el.margin = {m, m, m, m};
    el.border = {p, p, p, p};
    el.align_width = align.x;
    el.align_height = align.y;

    return el;
}

AutoLayoutElement make_auto(AutoLayout layout, Vector2 align = {})
{
    AutoLayoutElement el = {};
    el.layout = layout;
    el.align_width = align.x;
    el.align_height = align.y;
    return el;
}

GuiMainMenu GuiMainMenu::init(UiState *ui_state)
{
    return {Normal, ui_state};
}

void GuiMainMenu::deinit()
{
    // Nothing yet
}

static bool big_menu_button(UiUser &user, const char *text, bool major = false)
{
    float size = major ? 256 : 192;
    float margin = (256 - size) / 2;

    AutoLayoutElement el = make_element({AutoLayout::Row}, {size, size}, false,
                                        false, {0.5, 0.5}, 0, margin);
    begin_button_frame(user, text, el);
    label(user, text, {3, 3});
    return end_button_frame(user);
}

static void menu_settings(UiUser &user, float &scale_delta, GuiMainMenu &state)
{
    const char *buttons[] = {"Close", "Scale+", "Scale-", NULL};
    switch (msgbox(user, "Settings", "Change the scale of the UI.",
                   MsgBoxType::Info, buttons))
    {
    case 0:
        state.state = GuiMainMenu::Normal;
        break;
    case 1:
        scale_delta = 0.1;
        break;
    case 2:
        scale_delta = -0.1;
        break;
    }
}

static void menu_exit(UiUser &user, GuiMainMenu &state)
{
    const char *buttons[] = {"Yes", "No", NULL};
    switch (msgbox(user, "Exit", "Are you sure you want to exit?",
                   MsgBoxType::Warning, buttons))
    {
    case 0:
        sapp_request_quit();
        break;
    case 1:
        state.state = GuiMainMenu::Normal;
        break;
    }
}

int GuiMainMenu::show(UiUser &user, GuiTransition &transition)
{
    int exitcode = 1;

    user.begin_generic(
        make_element({AutoLayout::Row},
                     {sapp_widthf() / this->ui_state->dpi_scale,
                      sapp_heightf() / this->ui_state->dpi_scale},
                     false, false, {0.5, 0.5}, 0),
        UiMakeBrush::make_gradient({0, 0.0, 0.4, 1.0}, {0, 0.1, 0.6, 1.0}), {});

    user.begin_generic(make_auto({AutoLayout::Row}), {}, {});
    user.begin_generic(make_auto({AutoLayout::Column}, {0.5, 0}), {}, {});
    AutoLayoutElement el =
        make_element({AutoLayout::Row}, {0, 0}, true, true, {0.0, 0.0}, 3, 3);
    user.begin_generic(el, {}, {});

    user.bold = true;
    label(
        user, "STORYLAND", {2.5, 2.5},
        UiMakeBrush::make_gradient({0.2, 0.8, 0.0, 1.0}, {0.5, 1.0, 0.0, 1.0}));
    user.bold = false;

    user.end_generic();
    el = make_element({AutoLayout::Row}, {0, 0}, true, true, {0.0, 0.0}, 3, 3);
    user.begin_generic(el, {}, {});

    if (big_menu_button(user, "Settings"))
    {
        this->state = GuiMainMenu::Settings;
    }
    if (big_menu_button(user, "Play & Edit", true))
    {
        transition.begin();
    }
    if (big_menu_button(user, "Exit"))
    {
        this->state = GuiMainMenu::Exit;
    }

    user.end_generic();
    user.end_generic();
    user.end_generic();
    user.end_generic();

    float scale_delta = 0;
    switch (this->state)
    {
    case GuiMainMenu::Settings:
        menu_settings(user, scale_delta, *this);
        break;
    case GuiMainMenu::Exit:
        menu_exit(user, *this);
        break;
    default:
        break;
    }

    if (transition.switching())
    {
        exitcode = 2;
    }

    // FIXME: This will scale while the UI User is in the pass, but it will not
    // be applied until the next frame. This is a bug.
    user.state->dpi_scale += scale_delta;

    return exitcode;
}
