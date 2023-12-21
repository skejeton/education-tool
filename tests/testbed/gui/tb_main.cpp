#include <stdio.h>
#undef SOKOL_NO_ENTRY
#include "catedu/sys/sokol_setup.hpp"
#include "catedu/ui/user.hpp"

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
        user.label("Test test test test test test test test test\n");
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
