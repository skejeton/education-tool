#include "ui_autolayout.hpp"
#include "catedu/core/memory/init.hpp"
#include <algorithm>
#include <assert.h>

struct LayoutPassthrough
{
    AutoLayoutNodeId node;

    LayoutPassthrough then(AutoLayoutNodeId node) { return { node }; }
};

struct LayoutBuilder
{
    AutoLayoutProcess* process;
    AutoLayoutResult* last;
    BumpAllocator alloc;

    LayoutPassthrough begin() { return { this->process->root }; }
};

AutoLayoutResult*
alloc_result(LayoutBuilder& builder)
{
    auto* result = zeroinit(builder.alloc.alloc<AutoLayoutResult>());

    builder.last->next = result;
    builder.last = result;

    return result;
}

Vector2
recurse(LayoutBuilder& builder, LayoutPassthrough pass)
{
    AutoLayoutNode* node = builder.process->nodes.get(pass.node.id);
    assert(node);
    AutoLayoutElement el = node->element;
    Vector2 my_size = { 0, 0 };

    AutoLayoutNodeId child = node->child;
    if (!child.id.valid()) {
        return el.base_size;
    }

    while (child.id.valid()) {
        Vector2 size = recurse(builder, pass.then(child));
        AutoLayoutNode* child_node = builder.process->nodes.get(child.id);
        assert(child_node);

        switch (el.layout.type) {
            case AutoLayout::Column:
                my_size.x = std::max(size.x, my_size.x);
                my_size.y += size.y;
                break;
            case AutoLayout::Row:
                my_size.x += size.x;
                my_size.y = std::max(size.y, my_size.y);
                break;
        }

        child = builder.process->nodes.get(child.id)->sibling;
    }

    return my_size;
}

AutoLayoutProcess
AutoLayoutProcess::init(AutoLayoutNodeId& root)
{
    auto result = AutoLayoutProcess();

    // TODO: Properly initialize root element.
    root = result.root = { result.nodes.allocate({ 0 }) };

    return result;
}

void
AutoLayoutProcess::deinit()
{
    this->nodes.deinit();
    this->notes.deinit();
}

AutoLayoutNodeId
AutoLayoutProcess::add_element(AutoLayoutNodeId parent,
                               AutoLayoutElement element)
{
    AutoLayoutNode node;
    node.element = element;
    node.child = { 0 };
    node.sibling = { 0 };

    AutoLayoutNode* parent_node = this->nodes.get(parent.id);
    assert(parent_node);
    AutoLayoutNodeId child = { this->nodes.allocate(node) };
    assert(child.id.valid());
    if (parent_node->last.id.valid()) {
        AutoLayoutNode* last_node = this->nodes.get(parent_node->last.id);
        assert(last_node);
        last_node->sibling = child;
    }
    parent_node->last = child;

    return { child };
}

void
AutoLayoutProcess::process(BumpAllocator alloc, AutoLayoutResult& result)
{
    LayoutBuilder builder = {};
    builder.process = this;
    builder.last = &result;
    builder.alloc = alloc;

    recurse(builder, builder.begin());
}
