#pragma once
#include "catedu/core/storage/stack.hpp"
struct ScriptStr
{
    char str[128];
};

struct Script
{
    Stack<ScriptStr> things;
};
