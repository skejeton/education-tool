///
/// Dispatcher allows to store history of commands and undo/redo them.
///

#pragma once

#include "catedu/core/alloc/free_list.hpp"
#include "world.hpp"

struct EditOp
{
    enum class Type
    {
        Noop,
        Place,
        Remove
    };

    Type type;
    Object object;
    Place *place;

    EditOp *next;
    EditOp *prev;
};

struct Dispatcher
{
    World world;
    FreeList<EditOp> history;
    EditOp *current;
    bool dirty;

    static Dispatcher create();
    void destroy();

    void place_object(Object object);
    void remove_object(int x, int y);

    void enter_place(Object *object);

    void undo();
    void redo();
};
