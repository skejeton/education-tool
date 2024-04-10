#pragma once
#include "catedu/core/storage/stack.hpp"
#include "catedu/ui/user.hpp"

struct Dialog
{
    char name[64];
    char text[1024];
};

struct DialogList
{
    Stack<Dialog> dialogs;
};

struct DialogEditor
{
    int selected;

    static DialogEditor init();
    void deinit();

    void show(UiUser &user, DialogList &list);
};
