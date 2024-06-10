#include "debugtree.hpp"
#include "catedu/ui/rendering/make_brush.hpp"
#include "catedu/ui/widgets.hpp"

void alloc_debug_entry(GuiDebugTree *tree, const char *name,
                       uint32_t name_color, const char *value,
                       uint32_t value_color)
{
    DebugEntry *entry = tree->allocator.alloct<DebugEntry>();
    char *name_slot = (char *)tree->allocator.alloc(strlen(name) + 1);
    char *value_slot = (char *)tree->allocator.alloc(strlen(value) + 1);
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

    result.allocator = Arena::create(&ALLOCATOR_MALLOC);

    return result;
}

void GuiDebugTree::deinit()
{
    this->allocator.destroy();
    this->entry = nullptr;
}

void GuiDebugTree::reset()
{
    this->allocator.reset();
    this->entry = nullptr;
}

void GuiDebugTree::size(const char *name, size_t value)
{
    size_t bytes = value;
    size_t kibibytes = value / 1024;
    size_t mebibytes = kibibytes / 1024;
    size_t gibibytes = mebibytes / 1024;

    char buffer[256];

    if (gibibytes > 0)
    {
        snprintf(buffer, 256, "%zu GiB (%zu B)", gibibytes, bytes);
        alloc_debug_entry(this, name, 0x000000FF, buffer, 0x000077FF);
    }
    else if (mebibytes > 0)
    {
        snprintf(buffer, 256, "%zu MiB (%zu B)", mebibytes, bytes);
        alloc_debug_entry(this, name, 0x000000FF, buffer, 0x000077FF);
    }
    else if (kibibytes > 0)
    {
        snprintf(buffer, 256, "%zu KiB (%zu B)", kibibytes, bytes);
        alloc_debug_entry(this, name, 0x000000FF, buffer, 0x000077FF);
    }
    else
    {
        snprintf(buffer, 256, "%zu B", bytes);
        alloc_debug_entry(this, name, 0x000000FF, buffer, 0x000077FF);
    }
}

void GuiDebugTree::value(const char *name, int64_t value)
{
    char buffer[256];
    snprintf(buffer, 256, "%lli", value);
    alloc_debug_entry(this, name, 0x000000FF, buffer, 0x000077FF);
}

void GuiDebugTree::value(const char *name, uint64_t value)
{
    char buffer[256];
    snprintf(buffer, 256, "%llu", value);
    alloc_debug_entry(this, name, 0x000000FF, buffer, 0x000077FF);
}

void GuiDebugTree::value(const char *name, float value)
{
    char buffer[256];
    snprintf(buffer, 256, "%g", value);
    alloc_debug_entry(this, name, 0x000000FF, buffer, 0x000077FF);
}

void GuiDebugTree::value(const char *name, double value)
{
    char buffer[256];
    snprintf(buffer, 256, "%g", value);
    alloc_debug_entry(this, name, 0x000000FF, buffer, 0x000077FF);
}

void GuiDebugTree::value(const char *name, bool value)
{
    char buffer[256];
    snprintf(buffer, 256, "%s", value ? "true" : "false");
    alloc_debug_entry(this, name, 0x000000FF, buffer,
                      value ? 0x007700FF : 0x770000FF);
}

void GuiDebugTree::value(const char *name, const char *value)
{
    alloc_debug_entry(this, name, 0x000000FF, value, 0x777700FF);
}

void GuiDebugTree::show(UiPass &user)
{
    AutoLayoutElement wrapper = {};
    wrapper.layout.type = AutoLayout::row;
    user.begin_generic(wrapper, {}, {});

    AutoLayoutElement el = {};
    el.layout.type = AutoLayout::column;
    user.begin_generic(el, {}, {});

    for (DebugEntry *entry = this->entry; entry; entry = entry->next)
    {
        label(user, entry->name, {1, 1},
              UiMakeBrush::make_solid(entry->name_color));
    }

    user.end_generic();

    el.width.type = AutoLayoutDimension::pixel;
    el.width.value = 25;
    user.begin_generic(el, {}, {});
    user.end_generic();

    el.width.type = AutoLayoutDimension::autom;
    el.width.value = 0;

    user.begin_generic(el, {}, {});

    for (DebugEntry *entry = this->entry; entry; entry = entry->next)
    {
        label(user, entry->value, {1, 1},
              UiMakeBrush::make_solid(entry->value_color));
    }

    user.end_generic();

    user.end_generic();
}
