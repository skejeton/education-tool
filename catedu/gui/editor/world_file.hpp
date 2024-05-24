#pragma once

#include "catedu/gui/editor/dispatcher.hpp"
struct WorldFile
{
    static void save(const char *path, Dispatcher &dispatcher);
    static Dispatcher load(const char *path);
};
