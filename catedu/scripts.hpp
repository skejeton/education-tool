#include "catedu/gui/game/game.hpp"

struct ScriptData
{
    int data[32];
    bool activate_dialog;
    Vector2 target_pos;
    Dialog dialog;
    bool activate_grass;
    bool backtrack;
    int currency;
    int reals;
};

struct ScriptEvent
{
    char *script_name;
    bool walked_on;
    bool interacted;
    bool any;
};

char *run_script(ScriptData &scr, ScriptEvent event);
