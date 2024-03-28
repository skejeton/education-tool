#pragma once
#include "catedu/core/storage/table.hpp"
#include "catedu/ui/user.hpp"

struct Dialog
{
    char name[64];
    char text[1024];
};

struct DialogList
{
    Table<Dialog> dialogs;
};

struct DialogEditor
{
    static DialogEditor init();
    void deinit();

    void show(UiUser &user, DialogList &list);
};
