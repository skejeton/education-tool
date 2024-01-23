#include <stdio.h>
#undef SOKOL_NO_ENTRY
#include "catedu/sys/sokol_setup.hpp"
#include "catedu/ui/rendering/make_brush.hpp"
#include "catedu/ui/user.hpp"
#include "catedu/ui/widgets.hpp"

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
        begin_show_window(user, {"Entities", {100, 100, 200, 200}});
        user.button("Hello, world!");
        end_show_window(user);
        begin_show_window(user, {"Test", {0, 0, 600, 50}});
        user.button("Hello, wordld!");
        end_show_window(user);

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
