#pragma once
#include "catedu/ui/user.hpp"
#include <stdint.h>

struct GuiDebugTree
{
    char *str;
    size_t str_len;

    static GuiDebugTree init();
    void deinit();

    void reset();

    void value(char *name, int64_t value);
    void value(char *name, uint64_t value);
    void value(char *name, float value);
    void value(char *name, double value);
    void value(char *name, bool value);
    void value(char *name, const char *value);

    void show(UiUser &user);
};
