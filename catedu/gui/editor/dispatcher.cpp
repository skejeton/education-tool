#include "dispatcher.hpp"
#include "catedu/core/alloc/allocator.hpp"

void unperform_op(EditOp &op, World *world)
{
    switch (op.type)
    {
    case EditOp::Type::Place:
        assert(world->get_object_at(op.object.x, op.object.y) != nullptr);
        world->remove_object(op.object.x, op.object.y);
        return;
        break;
    case EditOp::Type::Remove:
        assert(world->place_object(op.object) != nullptr);
        return;
        break;
    case EditOp::Type::Noop:
        assert(false);
        break;
    }

    assert(false);
}

bool perform_op(EditOp &op, World *world)
{
    switch (op.type)
    {
    case EditOp::Type::Place:
        return world->place_object(op.object) != nullptr;
        break;
    case EditOp::Type::Remove:
        if (world->get_object_at(op.object.x, op.object.y) == nullptr)
        {
            return false;
        }
        world->remove_object(op.object.x, op.object.y);
        return true;
        break;
    case EditOp::Type::Noop:
        assert(false);
        break;
        break;
    }

    assert(false);
    return false;
}

static void append(Dispatcher &disp, EditOp op)
{
    assert(disp.current);

    EditOp *alloc = disp.history.alloc();
    *alloc = op;

    EditOp *orphan = disp.current->next;
    while (orphan)
    {
        disp.history.free(orphan);
        orphan = orphan->next;
    }

    disp.current->next = alloc;
    alloc->prev = disp.current;
    disp.current = alloc;
}

Dispatcher Dispatcher::create()
{
    Dispatcher dispatcher = {};
    dispatcher.history =
        FreeList<EditOp>::create(Arena::create(&ALLOCATOR_MALLOC));
    dispatcher.current = dispatcher.history.alloc();
    *dispatcher.current = {};
    dispatcher.current->type = EditOp::Type::Noop;
    dispatcher.world = World::create();
    return dispatcher;
}

void Dispatcher::destroy()
{
    world.destroy();
    history.destroy();
}

void Dispatcher::place_object(Object object)
{
    EditOp op = {};
    op.type = EditOp::Type::Place;
    op.object = object;

    if (object.type == Object::Type::Player)
    {
        for (auto &obj : iter(world.objects))
        {
            if (obj.type == Object::Type::Player)
            {
                remove_object(obj.x, obj.y);
            }
        }
    }

    if (perform_op(op, &world))
    {
        dirty = true;
        append(*this, op);
    }
}

void Dispatcher::remove_object(int x, int y)
{
    Object *obj = world.get_object_at(x, y);
    if (obj == nullptr)
    {
        return;
    }

    EditOp op = {};
    op.type = EditOp::Type::Remove;
    op.object = *obj;
    if (perform_op(op, &world))
    {
        dirty = true;
        append(*this, op);
    }
}

void Dispatcher::undo()
{
    if (!current || !current->prev)
    {
        return;
    }

    dirty = true;
    unperform_op(*current, &world);

    current = current->prev;
}

void Dispatcher::redo()
{
    if (!current || !current->next)
    {
        return;
    }

    current = current->next;

    dirty = true;
    assert(perform_op(*current, &world));
}
