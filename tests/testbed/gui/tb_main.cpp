#include <stdio.h>
#undef SOKOL_NO_ENTRY
#include "catedu/sys/sokol_setup.hpp"

struct TestEntry : SokolSetup
{
    void init()
    {

    }

    void frame()
    {

    }

    void cleanup()
    {

    }

    void input(const sapp_event *event)
    {

    }
};

SOKOL_SETUP_QUICK_ENTRYPOINT(TestEntry, "GUI Test");
