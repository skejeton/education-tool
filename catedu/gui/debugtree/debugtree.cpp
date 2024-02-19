#include "debugtree.hpp"
#include "catedu/sys/oom.hpp"
#include "catedu/ui/widgets.hpp"
#include <stdio.h>
#include <stdlib.h>

#define MAX_CHARS 4096

GuiDebugTree GuiDebugTree::init()
{
    GuiDebugTree result = {};

    result.str = (char *)OOM_HANDLER(malloc(MAX_CHARS));

    return result;
}

void GuiDebugTree::deinit()
{
    free(this->str);
}

void GuiDebugTree::reset()
{
    this->str_len = 0;
    this->str[0] = 0;
}

void GuiDebugTree::value(char *name, int64_t value)
{
    this->str_len +=
        snprintf(this->str + this->str_len, MAX_CHARS - this->str_len - 1,
                 "%s %lld\n", name, value);
}

void GuiDebugTree::value(char *name, uint64_t value)
{
    this->str_len +=
        snprintf(this->str + this->str_len, MAX_CHARS - this->str_len - 1,
                 "%s %llu\n", name, value);
}

void GuiDebugTree::value(char *name, float value)
{
    this->str_len +=
        snprintf(this->str + this->str_len, MAX_CHARS - this->str_len - 1,
                 "%s %g\n", name, value);
}

void GuiDebugTree::value(char *name, double value)
{
    this->str_len +=
        snprintf(this->str + this->str_len, MAX_CHARS - this->str_len - 1,
                 "%s %lg\n", name, value);
}

void GuiDebugTree::value(char *name, bool value)
{
    this->str_len +=
        snprintf(this->str + this->str_len, MAX_CHARS - this->str_len - 1,
                 "%s %s\n", name, value ? "true" : "false");
}

void GuiDebugTree::value(char *name, const char *value)
{
    this->str_len +=
        snprintf(this->str + this->str_len, MAX_CHARS - this->str_len - 1,
                 "%s %s\n", name, value);
}

void GuiDebugTree::show(UiUser &user)
{
    label(user, this->str);
}
