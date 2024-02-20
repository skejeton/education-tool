#pragma once

#include "catedu/ui/user.hpp"

enum
{
    DATABASE,
    LEARNING,
    LEARNING2
};

struct GuiText
{
    bool show_something;
    char guess[100];
    int word;
    int mode;
    bool revealed;

    static GuiText init()
    {
        return {};
    }

    void deinit()
    {
    }

    void show(UiUser &user, const char *text);
};
