#include "script.hpp"
#include "catedu/core/alloc/allocator.hpp"
#include "catedu/core/alloc/arena.hpp"
#include "catedu/core/memory/addressfixer.hpp"

ScriptNode *Script::append_node(ScriptNode::Type t, ScriptNode *parent)
{
    ScriptNode *result = nodes.alloc();
    *result = {};
    result->parent = parent;

    if (parent->type == ScriptNode::Type::event)
    {
        ScriptNode *last = parent;
        while (1)
        {
            if (last->next)
            {
                last = last->next;
            }
            else
            {
                break;
            }
        }
        last->next = result;
    }

    result->type = t;
    return result;
}

Script Script::clone()
{
    Script result = {};
    result.nodes =
        FreeList<ScriptNode>::create(Arena::create(&ALLOCATOR_MALLOC));

    AddressFixer<ScriptNode> fixer = AddressFixer<ScriptNode>::create();
    for (auto &a : iter(nodes))
    {
        ScriptNode *b = result.nodes.alloc(a);

        if (&a == root)
        {
            result.root = &a;
            fixer.add_pointer(&result.root);
        }

        fixer.add_mapping(&a, b);
        fixer.add_pointer(&b->parent);
        fixer.add_pointer(&b->next);
        if (b->type == ScriptNode::Type::yesno)
        {
            fixer.add_pointer(&b->yesno.yes);
            fixer.add_pointer(&b->yesno.no);
        }
    }

    fixer.fix_addresses();
    fixer.destroy();

    return result;
}

Script Script::create(Arena arena)
{
    return {FreeList<ScriptNode>::create(arena)};
}

void Script::destroy()
{
    nodes.destroy();
}
