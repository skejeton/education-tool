#include "debugtree.hpp"
#include "catedu/sys/oom.hpp"
#include "catedu/ui/rendering/colors.hpp"
#include "catedu/ui/widgets.hpp"

void alloc_debug_entry(GuiDebugTree *tree, const char *name,
                       uint32_t name_color, const char *value,
                       uint32_t value_color)
{
    DebugEntry *entry = tree->allocator.alloct<DebugEntry>();
    OOM_HANDLER(entry);
    char *name_slot = (char *)tree->allocator.alloc(strlen(name) + 1);
    OOM_HANDLER(name_slot);
    char *value_slot = (char *)tree->allocator.alloc(strlen(value) + 1);
    OOM_HANDLER(value_slot);
    memcpy(name_slot, name, strlen(name) + 1);
    memcpy(value_slot, value, strlen(value) + 1);

    entry->name = name_slot;
    entry->name_color = name_color;
    entry->value = value_slot;
    entry->value_color = value_color;

    entry->next = tree->entry;
    tree->entry = entry;
}

GuiDebugTree GuiDebugTree::init()
{
    GuiDebugTree result = {};

    result.allocator = Arena::create_malloc();

    return result;
}

void GuiDebugTree::deinit()
{
    this->allocator.destroy();
}

void GuiDebugTree::reset()
{
    this->allocator.reset();
    this->entry = nullptr;
}

void GuiDebugTree::value(const char *name, int64_t value)
{
    alloc_debug_entry(this, name, 0x000000FF, stdstrfmt("%lli", value).c_str(),
                      0x000077FF);
}

void GuiDebugTree::value(const char *name, uint64_t value)
{
    alloc_debug_entry(this, name, 0x000000FF, stdstrfmt("%llu", value).c_str(),
                      0x000077FF);
}

void GuiDebugTree::value(const char *name, float value)
{
    alloc_debug_entry(this, name, 0x000000FF, stdstrfmt("%g", value).c_str(),
                      0x000077FF);
}

void GuiDebugTree::value(const char *name, double value)
{
    alloc_debug_entry(this, name, 0x000000FF, stdstrfmt("%g", value).c_str(),
                      0x000077FF);
}

void GuiDebugTree::value(const char *name, bool value)
{
    alloc_debug_entry(this, name, 0x000000FF,
                      stdstrfmt("%s", value ? "true" : "false").c_str(),
                      value ? 0x007700FF : 0x770000FF);
}

void GuiDebugTree::value(const char *name, const char *value)
{
    alloc_debug_entry(this, name, 0x000000FF, value, 0x777700FF);
}

void GuiDebugTree::show(UiUser &user)
{
    AutoLayoutElement wrapper = {};
    wrapper.layout.type = AutoLayout::Row;
    user.begin_generic(wrapper, {}, {});

    AutoLayoutElement el = {};
    el.layout.type = AutoLayout::Column;
    user.begin_generic(el, {}, {});

    for (DebugEntry *entry = this->entry; entry; entry = entry->next)
    {
        label(user, entry->name, {1, 1},
              UiMakeBrush::make_solid(to_color(entry->name_color)));
    }

    user.end_generic();

    el.width.type = AutoLayoutDimension::Pixel;
    el.width.value = 25;
    user.begin_generic(el, {}, {});
    user.end_generic();

    el.width.type = AutoLayoutDimension::Auto;
    el.width.value = 0;

    user.begin_generic(el, {}, {});

    for (DebugEntry *entry = this->entry; entry; entry = entry->next)
    {
        label(user, entry->value, {1, 1},
              UiMakeBrush::make_solid(to_color(entry->value_color)));
    }

    user.end_generic();

    user.end_generic();
}
