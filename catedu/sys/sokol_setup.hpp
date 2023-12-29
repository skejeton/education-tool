#ifndef HPP_CATEDU_SYS_SETUP
#define HPP_CATEDU_SYS_SETUP

#include "sokol/sokol_app.h"
#include "sokol/sokol_log.h"

#define SOKOL_SETUP_QUICK_ENTRYPOINT(Type, title)\
    sapp_desc sokol_main(int argc, char *argv[])\
    {\
        (void) argc; (void) argv;\
        return sokol_setup_default(title, new Type());\
    }

struct SokolSetup {
    virtual void init() = 0;
    virtual void frame() = 0;
    virtual void cleanup() = 0;
    virtual void input(const sapp_event *event) = 0;
};

sapp_desc sokol_setup_default(const char *title, SokolSetup *setup);

#endif
