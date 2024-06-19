#pragma once
#include "catedu/core/storage/stack.hpp"

struct ScriptNode
{
    char message[256];
};

struct Script
{
    Stack<ScriptNode> nodes;

    Script clone();
    void destroy();
};
