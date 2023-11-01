#include "ui_autolayout.hpp"
#include "catedu/core/memory/init.hpp"
#include <algorithm>
#include <assert.h>


struct ResultBuilder
{
    AutoLayoutProcess* process;
    AutoLayoutResult* last;
    BumpAllocator *alloc;

    AutoLayoutNodeId begin() { return this->process->root; }
};

AutoLayoutResult*
alloc_result(ResultBuilder& builder)
{
    auto* result = zeroinit(builder.alloc->alloc<AutoLayoutResult>());

    builder.last->next = result;
    builder.last = result;

    return result;
}

Vector2
recurse(AutoLayoutProcess *process, AutoLayoutNodeId n)
{
    AutoLayoutNode* node = process->nodes.get(n.id);
    assert(node);
    AutoLayoutElement el = node->element;
    Vector2 my_size = { 0, 0 };

    AutoLayoutNodeId child = node->child;
    if (!child.id.valid()) {
        my_size = el.base_size;
        goto done;
    }

    while (child.id.valid()) {
        Vector2 size = recurse(process, child);
        AutoLayoutNode* child_node = process->nodes.get(child.id);
        assert(child_node);

        switch (el.layout.type) {
            case AutoLayout::Column:
                child_node->element.calc_rect.pos += { 0, my_size.y };
                my_size.y += size.y;
                my_size.x = std::max(size.x, my_size.x);
                break;
            case AutoLayout::Row:
                child_node->element.calc_rect.pos += { my_size.x, 0 };
                my_size.x += size.x;
                my_size.y = std::max(size.y, my_size.y);
                break;
        }

        child = child_node->sibling;
    }

done:
    node->element.calc_rect.siz = my_size;
    return my_size;
}

void
align_to_parents(AutoLayoutProcess* process, AutoLayoutNodeId n)
{
    AutoLayoutNode* node = process->nodes.get(n.id);
    assert(node);
    AutoLayoutNodeId child = node->child;

    while (child.id.valid()) {
        AutoLayoutNode* child_node = process->nodes.get(child.id);
        assert(child_node);

        child_node->element.calc_rect.pos += node->element.calc_rect.pos;
        align_to_parents(process, child);

        child = child_node->sibling;
    }
}



void
build_results(ResultBuilder &builder, AutoLayoutNodeId n)
{
    AutoLayoutNode* node = builder.process->nodes.get(n.id);
    assert(node);

    AutoLayoutResult *result = alloc_result(builder);
    result->rect = node->element.calc_rect;
    result->userdata = node->element.userdata;
    
    AutoLayoutNodeId child = node->child;
    while (child.id.valid()) {
        AutoLayoutNode* child_node = builder.process->nodes.get(child.id);
        assert(child_node);
        build_results(builder, child);
        child = child_node->sibling;
    }
}

AutoLayoutProcess
AutoLayoutProcess::init(AutoLayoutNodeId& root)
{
    auto result = AutoLayoutProcess();

    AutoLayoutNode node = { 0 };
    node.element.layout = { AutoLayout::Row };

    // TODO: Properly initialize root element.
    root = result.root = { result.nodes.allocate(node) };

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
    AutoLayoutNode node = {};
    node.element = element;
    node.parent = parent;
    node.child = { 0 };
    node.sibling = { 0 };

    AutoLayoutNode* parent_node = this->nodes.get(parent.id);
    assert(parent_node);
    AutoLayoutNodeId child = { this->nodes.allocate(node) };
    assert(child.id.valid());

    if (!parent_node->last.id.valid()) {
        parent_node->child = child;
    } else {
        AutoLayoutNode* last_node = this->nodes.get(parent_node->last.id);
        assert(last_node);
        last_node->sibling = child;
    }

    parent_node->last = child;

    return { child };
}

void
AutoLayoutProcess::process(BumpAllocator alloc, AutoLayoutResult*& result)
{
    ResultBuilder builder = {};
    builder.process = this;
    builder.last = result = zeroinit(alloc.alloc<AutoLayoutResult>());
    builder.alloc = &alloc;

    recurse(this, builder.begin());
    align_to_parents(this, builder.begin());
    build_results(builder, builder.begin());
}
