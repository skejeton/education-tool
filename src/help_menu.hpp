///
/// BRIEF: Help menu
///

#ifndef H_CATEDU_HELP_MENU
#define H_CATEDU_HELP_MENU

#include "easy_gui.hpp"

struct HelpMenu {
    bool was_init;
    bool shown;

    void show(EasyGui *gui);
};

#endif // H_CATEDU_HELP_MENU