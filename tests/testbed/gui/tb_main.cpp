#include <stdio.h>
#undef SOKOL_NO_ENTRY
#include "catedu/gui/text/text.hpp"
#include "catedu/sys/sokol_setup.hpp"
#include "catedu/ui/rendering/make_brush.hpp"
#include "catedu/ui/user.hpp"
#include "catedu/ui/widgets.hpp"
#include <sokol/sokol_gfx.h>

const char *FONT = "assets/Roboto-Regular.ttf";
const char *BOLD_FONT = "assets/Roboto-Bold.ttf";

struct TestEntry : SokolSetup
{
    GuiText gui_text;
    UiState ui;

    void init()
    {
        this->ui = UiState::init(FONT, BOLD_FONT, sapp_dpi_scale());
        this->gui_text = GuiText::init();

        // @Hack
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
