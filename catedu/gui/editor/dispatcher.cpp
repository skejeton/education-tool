#include "dispatcher.hpp"
#include "catedu/core/alloc/allocator.hpp"

void unperform_op(EditOp &op, World *world)
{
    switch (op.type)
    {
    case EditOp::Type::Place:
        assert(world->current->get_object_at(op.object.x, op.object.y) !=
               nullptr);
        world->current->remove_object(op.object.x, op.object.y);
        return;
        break;
    case EditOp::Type::Remove:
        assert(world->current->place_object(op.object) != nullptr);
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
        return world->current->place_object(op.object) != nullptr;
        break;
    case EditOp::Type::Remove:
        if (world->current->get_object_at(op.object.x, op.object.y) == nullptr)
        {
            return false;
        }
        world->current->remove_object(op.object.x, op.object.y);
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
    op.place = disp.world.current;

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
        for (auto &obj : iter(world.current->objects))
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
    Object *obj = world.current->get_object_at(x, y);
    if (obj == nullptr)
    {
        return;
    }

    if (obj->place && obj->place != world.first)
    {
        obj->place->destroy();
        world.places.free(obj->place);
    }

    obj->place = nullptr;

    EditOp op = {};
    op.type = EditOp::Type::Remove;
    op.object = *obj;
    op.object.x = x;
    op.object.y = y;
    if (perform_op(op, &world))
    {
        dirty = true;
        append(*this, op);
    }
}

void Dispatcher::enter_place(Object *object)
{
    if (object->place == nullptr)
    {
        object->place = world.places.alloc();
        *object->place = Place::create();
        object->place->interior = true;
    }

    world.current = object->place;
}

void Dispatcher::undo()
{
    if (!current || !current->prev || current->place != world.current)
    {
        return;
    }

    dirty = true;
    unperform_op(*current, &world);

    current = current->prev;
}

void Dispatcher::redo()
{
    if (!current || !current->next || current->place != world.current)
    {
        return;
    }

    current = current->next;

    dirty = true;
    assert(perform_op(*current, &world));
}
