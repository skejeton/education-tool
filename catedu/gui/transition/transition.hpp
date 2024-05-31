#pragma once

#include "catedu/ui/user.hpp"
struct GuiTransition
{
    float alpha;
    float alpha_target;

    static GuiTransition create();
    void destroy();

    void begin();
    bool going();
    bool switching();

    void show(UiUser &user, Input &input);
};
