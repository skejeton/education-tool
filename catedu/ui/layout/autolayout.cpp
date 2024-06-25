#include "autolayout.hpp"
#include <algorithm>
#include <assert.h>

struct ResultBuilder
{
    AutoLayoutProcess *process;
    AutoLayoutResult *last;
    Arena *alloc;
};

AutoLayoutResult *alloc_result(ResultBuilder &builder)
{
    auto *result = builder.alloc->alloc(AutoLayoutResult{});

    builder.last->next = result;
    builder.last = result;

    return result;
}

void recurse(AutoLayoutProcess *process, AutoLayoutNode *node)
{
    assert(node);
    AutoLayoutElement el = node->element;
    if (el.hidden)
    {
        return;
    }
    Vector2 my_size = {0, 0};

    AutoLayoutNode *child = node->child;

    while (child != nullptr)
    {
        recurse(process, child);

        AutoLayoutElement &cel = child->element;

        Vector2 size = cel.margin_box.siz;
        Vector2 delta = {0, 0};

        if (cel.position != AutoLayoutPosition::absolute &&
            cel.position != AutoLayoutPosition::detached)
        {
            switch (el.layout.type)
            {
            case AutoLayout::column:
                delta = {0, my_size.y};
                my_size.y += size.y;
                my_size.x = std::max(size.x, my_size.x);
                break;
            case AutoLayout::row:
                delta = {my_size.x, 0};
                my_size.x += size.x;
                my_size.y = std::max(size.y, my_size.y);
                break;
            }
        }

        child->element.margin_box.pos += delta;
        child->element.border_box.pos += delta;
        child->element.base_box.pos += delta;
        child->element.padding_box.pos += delta;

        child = child->sibling;
    }

    node->element.content_box.siz = my_size;

    if (el.width.type == AutoLayoutDimension::Type::pixel)
    {
        my_size.x = el.width.value;
    }
    if (el.height.type == AutoLayoutDimension::Type::pixel)
    {
        my_size.y = el.height.value;
    }

    node->element.padding_box.siz = my_size;
    node->element.base_box.siz = el.padding.apply_size(my_size);
    node->element.border_box.siz =
        el.border.apply_size(node->element.base_box.siz);
    node->element.margin_box.siz =
        el.margin.apply_size(node->element.border_box.siz);
    node->element.border_box.pos += {el.margin.l, el.margin.t};
    node->element.base_box.pos +=
        Vector2{el.border.l, el.border.t} + node->element.border_box.pos;
    node->element.padding_box.pos +=
        Vector2{el.padding.l, el.padding.t} + node->element.base_box.pos;
}

void align_to_parents(AutoLayoutProcess *process, AutoLayoutNode *node)
{
    assert(node);
    if (node->element.hidden)
    {
        return;
    }

    AutoLayoutNode *child = node->child;

    while (child != nullptr)
    {
        Vector2 delta = {0, 0};

        if (child->element.position != AutoLayoutPosition::absolute &&
            child->element.position != AutoLayoutPosition::detached)
        {
            child->element.padding_box.pos += node->element.padding_box.pos;
            child->element.base_box.pos += node->element.padding_box.pos;
            child->element.border_box.pos += node->element.padding_box.pos;
            child->element.margin_box.pos += node->element.padding_box.pos;
            if (node->element.layout.type == AutoLayout::row)
            {
                node->element.content_box.siz.y =
                    child->element.margin_box.siz.y;
            }
            else
            {
                node->element.content_box.siz.x =
                    child->element.margin_box.siz.x;
            }

            delta =
                (node->element.padding_box.siz -
                 node->element.content_box.siz) *
                Vector2{node->element.align_width, node->element.align_height};
        }

        delta += child->element.offset;
        delta += node->element.scroll;

        child->element.padding_box.pos += delta;
        child->element.base_box.pos += delta;
        child->element.border_box.pos += delta;
        child->element.margin_box.pos += delta;
        align_to_parents(process, child);

        child = child->sibling;
    }
}

// FIXME: Using a giant clip rect for now.
AutoLayoutResult *build_results(ResultBuilder &builder, AutoLayoutNode *node)
{
    assert(node);

    if (node->element.hidden)
    {
        return nullptr;
    }

    AutoLayoutResult *result = alloc_result(builder);

    result->padding_box = node->element.padding_box;
    result->base_box = node->element.base_box;
    result->border_box = node->element.border_box;
    result->margin_box = node->element.margin_box;

    result->userdata = node->element.userdata;

    if (node->element.pop)
    {
        result->pop = true;
    }

    if (node->element.clip)
    {
        result->clip = true;
    }

    AutoLayoutResult *last_sibling = nullptr;
    AutoLayoutNode *child = node->child;
    while (child != nullptr)
    {
        AutoLayoutResult *child_result = build_results(builder, child);
        if (child_result)
        {
            if (last_sibling)
            {
                last_sibling->sibling = child_result;
            }
            else
            {
                result->child = child_result;
            }
            last_sibling = child_result;
        }
        child = child->sibling;
    }

    return result;
}

AutoLayoutProcess AutoLayoutProcess::init(Arena &arena, AutoLayoutNode **root)
{
    AutoLayoutProcess result = {};
    result.arena = &arena;

    AutoLayoutNode node = {0};
    node.element.layout = {AutoLayout::row};

    // TODO: Properly initialize root element.
    result.root = arena.alloct<AutoLayoutNode>();
    assert(result.root != nullptr);
    *result.root = node;

    *root = result.root;

    return result;
}

void AutoLayoutProcess::deinit()
{
}

AutoLayoutNode *AutoLayoutProcess::add_element(AutoLayoutNode *parent,
                                               AutoLayoutElement element)
{
    AutoLayoutNode node = {};
    node.element = element;
    node.parent = parent;
    node.child = {0};
    node.sibling = {0};

    AutoLayoutNode *child = this->arena->alloct<AutoLayoutNode>();
    assert(child != nullptr);
    *child = node;

    if (!parent->last)
    {
        parent->child = child;
    }
    else
    {
        parent->last->sibling = child;
    }

    parent->last = child;

    return child;
}

void AutoLayoutProcess::process(Arena &alloc, AutoLayoutResult *&result)
{
    ResultBuilder builder = {};
    builder.process = this;
    builder.last = alloc.alloc(AutoLayoutResult{});
    builder.alloc = &alloc;

    recurse(this, builder.process->root);
    align_to_parents(this, builder.process->root);
    result = build_results(builder, builder.process->root);
}
