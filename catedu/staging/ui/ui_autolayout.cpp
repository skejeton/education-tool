#include "ui_autolayout.hpp"
#include "catedu/core/memory/init.hpp"
#include <algorithm>
#include <assert.h>

struct ResultBuilder
{
    AutoLayoutProcess *process;
    AutoLayoutResult *last;
    BumpAllocator *alloc;

    AutoLayoutNodeId begin()
    {
        return this->process->root;
    }
};

AutoLayoutResult *alloc_result(ResultBuilder &builder)
{
    auto *result = zeroinit(builder.alloc->alloc<AutoLayoutResult>());

    builder.last->next = result;
    builder.last = result;

    return result;
}

void recurse(AutoLayoutProcess *process, AutoLayoutNodeId n)
{
    AutoLayoutNode *node = process->nodes.get(n.id);
    assert(node);
    AutoLayoutElement el = node->element;
    Vector2 my_size = {0, 0};

    AutoLayoutNodeId child = node->child;
    if (!child.id.valid())
    {
        goto done;
    }

    while (child.id.valid())
    {
        recurse(process, child);
        AutoLayoutNode *child_node = process->nodes.get(child.id);
        assert(child_node);

        AutoLayoutElement &cel = child_node->element;
        Vector2 size = cel.margin_box.siz;
        Vector2 delta;

        switch (el.layout.type)
        {
        case AutoLayout::Column:
            delta = {0, my_size.y};
            my_size.y += size.y;
            my_size.x = std::max(size.x, my_size.x);
            break;
        case AutoLayout::Row:
            delta = {my_size.x, 0};
            my_size.x += size.x;
            my_size.y = std::max(size.y, my_size.y);
            break;
        }

        child_node->element.margin_box.pos += delta;
        child_node->element.border_box.pos += delta;
        child_node->element.base_box.pos += delta;
        child_node->element.padding_box.pos += delta;

        child = child_node->sibling;
    }

done:
    if (el.width.type == AutoLayoutDimension::Type::Pixel)
    {
        my_size.x = el.width.value;
    }
    if (el.height.type == AutoLayoutDimension::Type::Pixel)
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

void align_to_parents(AutoLayoutProcess *process, AutoLayoutNodeId n)
{
    AutoLayoutNode *node = process->nodes.get(n.id);
    assert(node);
    AutoLayoutNodeId child = node->child;

    while (child.id.valid())
    {
        AutoLayoutNode *child_node = process->nodes.get(child.id);
        assert(child_node);

        child_node->element.padding_box.pos += node->element.padding_box.pos;
        child_node->element.base_box.pos += node->element.padding_box.pos;
        child_node->element.border_box.pos += node->element.padding_box.pos;
        child_node->element.margin_box.pos += node->element.padding_box.pos;

        Vector2 delta =
            (node->element.padding_box.siz -
             child_node->element.margin_box.siz) *
            Vector2{node->element.align_width, node->element.align_height};

        child_node->element.padding_box.pos += delta;
        child_node->element.base_box.pos += delta;
        child_node->element.border_box.pos += delta;
        child_node->element.margin_box.pos += delta;
        align_to_parents(process, child);

        child = child_node->sibling;
    }
}

void build_results(ResultBuilder &builder, AutoLayoutNodeId n)
{
    AutoLayoutNode *node = builder.process->nodes.get(n.id);
    assert(node);

    AutoLayoutResult *result = alloc_result(builder);
    result->padding_box = node->element.padding_box;
    result->base_box = node->element.base_box;
    result->border_box = node->element.border_box;
    result->margin_box = node->element.margin_box;
    result->userdata = node->element.userdata;

    AutoLayoutNodeId child = node->child;
    while (child.id.valid())
    {
        AutoLayoutNode *child_node = builder.process->nodes.get(child.id);
        assert(child_node);
        build_results(builder, child);
        child = child_node->sibling;
    }
}

AutoLayoutProcess AutoLayoutProcess::init(AutoLayoutNodeId &root)
{
    AutoLayoutProcess result = {};

    AutoLayoutNode node = {0};
    node.element.layout = {AutoLayout::Row};

    // TODO: Properly initialize root element.
    root = result.root = {result.nodes.allocate(node)};

    return result;
}

void AutoLayoutProcess::deinit()
{
    this->nodes.deinit();
    this->notes.deinit();
}

AutoLayoutNodeId AutoLayoutProcess::add_element(AutoLayoutNodeId parent,
                                                AutoLayoutElement element)
{
    AutoLayoutNode node = {};
    node.element = element;
    node.parent = parent;
    node.child = {0};
    node.sibling = {0};

    AutoLayoutNodeId child = {this->nodes.allocate(node)};
    assert(child.id.valid());
    AutoLayoutNode *parent_node = this->nodes.get(parent.id);
    assert(parent_node);

    if (!parent_node->last.id.valid())
    {
        parent_node->child = child;
    }
    else
    {
        AutoLayoutNode *last_node = this->nodes.get(parent_node->last.id);
        assert(last_node);
        last_node->sibling = child;
    }

    parent_node->last = child;

    return {child};
}

void AutoLayoutProcess::process(BumpAllocator alloc, AutoLayoutResult *&result)
{
    ResultBuilder builder = {};
    builder.process = this;
    builder.last = result = zeroinit(alloc.alloc<AutoLayoutResult>());
    builder.alloc = &alloc;

    recurse(this, builder.begin());
    align_to_parents(this, builder.begin());
    build_results(builder, builder.begin());
}
