///
/// BRIEF: Entry point for everything
///

#ifndef H_CATEDU_ENTRY
#define H_CATEDU_ENTRY

#include "boxdraw.hpp"
#include "camera.hpp"
#include "camera_input.hpp"
#include "console.hpp"
#include "gui/main_menu/main_menu.hpp"
#include "input.hpp"
#include "math.hpp"
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_glue.h"
#include "sokol/sokol_log.h"
#include "ui/rendering/font.hpp"
#include "ui/rendering/pass.hpp"
#include <memory>
#include <stdio.h>

struct Entry
{
    Input inputs;
    GuiMainMenu main_menu;

    void init();
    void input(const sapp_event* event);
    void cleanup();
    void frame();
};

#endif // H_CATEDU_ENTRY
