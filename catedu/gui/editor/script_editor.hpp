#pragma once
#include "catedu/scene/script.hpp"
#include "catedu/ui/user.hpp"

struct ScriptCardAction
{
    ScriptNode *parent;
    bool yes;
};

struct ScriptCardData
{
    bool dragging;
    Vector2 drag_offset;
    Vector2 drag_peg;
};

struct ScriptCardDragNDrop
{
    ScriptNode *node;
    ScriptNode *insert_after;
    ScriptNode *insert_after_final;
    ScriptCardData *dragging;
};

struct ScriptEditor
{
    Script *script;
    ScriptNode *current;
    ScriptCardDragNDrop dnd;

    void show(UiPass &user);
    static ScriptEditor create(Script *script);
};
