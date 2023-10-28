#include "ui_autolayout.hpp"
#include <assert.h>

void
recurse(AutoLayoutProcess* l, AutoLayoutNodeId start)
{
    AutoLayoutNode* node = l->nodes.get(start.id);
    assert(node);

    // etc etc handle calculations.

    while (node) {
        recurse(l, node->child);
        node = l->nodes.get(node->sibling.id);
    }
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

size_t
AutoLayoutProcess::process(BumpAllocator bump, AutoLayoutProcess*& dest)
{
    recurse(this, { 0 });
}
