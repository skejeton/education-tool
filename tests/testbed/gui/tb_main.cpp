#include <stdio.h>
#undef SOKOL_NO_ENTRY
#include "catedu/gui/text/text.hpp"
#include "catedu/sys/sokol_setup.hpp"
#include "catedu/ui/rendering/make_brush.hpp"
#include "catedu/ui/user.hpp"
#include "catedu/ui/widgets.hpp"
#include <sokol/sokol_gfx.h>

struct TestEntry : SokolSetup
{
    GuiText gui_text;
    UiState ui;

    void init()
    {
        this->ui = UiState::init("assets/Roboto-Regular.ttf", 3.0);
        this->gui_text = GuiText::init();
        this->ui.core->pass_action.colors->load_action = SG_LOADACTION_CLEAR;
    }

    void frame()
    {
        UiUser user = UiUser::init(this->ui);
        user.begin_pass();

        this->gui_text.show(user, "Hello, World!");

        user.end_pass();
    }

    void cleanup()
    {
        this->gui_text.deinit();
    }

    void input(const sapp_event *event)
    {
        this->ui.feed_event(event);
    }
};

SOKOL_SETUP_QUICK_ENTRYPOINT(TestEntry, "GUI Test");
