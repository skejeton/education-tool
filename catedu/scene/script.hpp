#pragma once
#include "catedu/core/alloc/arena.hpp"
#include "catedu/core/alloc/free_list.hpp"

struct ScriptNode
{
    enum class EventType
    {
        start,
        yes,
        no
    };

    enum class Type
    {
        event,
        say,
        yesno
    };

    EventType event;
    Type type;

    union {
        char say[256];
        struct
        {
            char question[256];
            ScriptNode *yes;
            ScriptNode *no;
        } yesno;
    };

    ScriptNode *parent;
    ScriptNode *next;
};

struct Script
{
    FreeList<ScriptNode> nodes;
    ScriptNode *root;

    ScriptNode *append_node(ScriptNode::Type t, ScriptNode *parent);

    Script clone();

    static Script create(Arena arena);
    void destroy();
};
