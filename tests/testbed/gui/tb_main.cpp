#include <stdio.h>
#undef SOKOL_NO_ENTRY
#include "catedu/sys/sokol_setup.hpp"
#include "catedu/ui/user.hpp"
#include "catedu/ui/rendering/make_brush.hpp"

struct TestEntry : SokolSetup
{
    UiState ui;

    void init()
    {
        this->ui = UiState::init("assets/Roboto-Regular.ttf");
    }

    void frame()
    {
        UiUser user = UiUser::init(this->ui);
        user.begin_pass();
        user.button("Hello, world!");
        AutoLayoutElement el = {};
        el.border = { 1, 1, 1, 1 };
        el.margin = { 5, 5, 5, 5 };
        el.padding = { 10, 10, 10, 10 };
        el.width.type = AutoLayoutDimension::Pixel;
        el.width.value = 200;
        el.height.type = AutoLayoutDimension::Pixel;
        el.height.value = 200;
        UiBrush border = UiMakeBrush::make_solid({0.5f, 0.5f, 0.5f, 1.0f});
        UiBrush background = UiMakeBrush::make_solid({0.0f, 0.0f, 0.0f, 1.0f});
        user.begin_generic(el, background, border);
        user.label("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\n");
        user.end_generic();
        user.end_pass();
    }

    void cleanup()
    {

    }

    void input(const sapp_event *event)
    {
        this->ui.feed_event(event);
    }
};

SOKOL_SETUP_QUICK_ENTRYPOINT(TestEntry, "GUI Test");
