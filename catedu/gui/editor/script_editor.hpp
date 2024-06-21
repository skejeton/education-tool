#pragma once
#include "catedu/scene/script.hpp"
#include "catedu/ui/user.hpp"

struct ScriptEditor
{
    Script *script;
    ScriptNode *current;

    void show(UiPass &user);
    static ScriptEditor create(Script *script);
};
