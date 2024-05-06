#pragma once
#include "catedu/core/alloc/bump_allocator.hpp"
#include "catedu/ui/user.hpp"
#include <stdint.h>

struct DebugEntry
{
    char *name;
    uint32_t name_color;
    char *value;
    uint32_t value_color;

    DebugEntry *next;
};

struct GuiDebugTree
{
    DebugEntry *entry;
    BumpAllocator allocator;

    static GuiDebugTree init();
    void deinit();

    void reset();

    void value(const char *name, int64_t value);
    void value(const char *name, uint64_t value);
    void value(const char *name, float value);
    void value(const char *name, double value);
    void value(const char *name, bool value);
    void value(const char *name, const char *value);

    void show(UiUser &user);
};
