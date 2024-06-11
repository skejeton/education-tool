#pragma once
#include "catedu/core/alloc/arena.hpp"
#include "catedu/core/math/math.hpp"

// Top, right, bottom, left.
struct TRBL
{
    float t, r, b, l;

    Rect apply_inflate(Rect rect)
    {
        return {{rect.pos - Vector2{l, t}}, rect.siz + Vector2{l + r, t + b}};
    }

    Rect apply_deflate(Rect rect)
    {
        return {{rect.pos + Vector2{l, t}}, rect.siz - Vector2{l + r, t + b}};
    }

    Rect apply_size(Rect rect)
    {
        return {{rect.pos}, rect.siz + Vector2{l + r, t + b}};
    }

    Vector2 apply_size(Vector2 size)
    {
        return size + Vector2{l + r, t + b};
    }
};

struct AutoLayout
{
    enum Type
    {
        column,
        row
    };

    Type type;
};

struct AutoLayoutDimension
{
    enum Type
    {
        autom,
        pixel
    };

    Type type;
    float value;
};

enum AutoLayoutPosition
{
    embedded, // Affects the parent's layout.
    relative, // Affects the parent's layout, positioned relative to the
              // parent.
    detached, // Doesn't affect the parent's layout, positioned relative to the
              // parent.
    absolute  // Doesn't affect the parent's layout, positioned relative to the
              // screen.
};

struct AutoLayoutElement
{
    void *userdata; // Reference to userdata.
    Vector2 offset;
    AutoLayoutDimension width;
    AutoLayoutDimension height;
    float align_width;
    float align_height;
    TRBL margin;
    TRBL border;
    TRBL padding;
    AutoLayout layout; // Layout-specific data.
    AutoLayoutPosition position;
    char *note; // Note for debugging.
    bool hidden;
    bool clip;
    bool pop;
    //----------------------------------------------- Calculated data.
    Rect padding_box; // Box inside padding.
    Rect base_box;    // Box inside border.
    Rect border_box;  // Box inside margin.
    Rect margin_box;  //
    Rect content_box; // Box inside padding that is the size of the content.
};

struct AutoLayoutResult
{
    void *userdata;
    Rect padding_box; // Box inside padding.
    Rect base_box;    // Box inside border.
    Rect border_box;  // Box inside margin.
    Rect margin_box;  //
    bool clip;
    char *note;
    AutoLayoutResult *next;
    AutoLayoutResult *child;
    AutoLayoutResult *sibling;
};

// Different from the `AutoLayoutElement` because it contains references to
// other nodes.
struct AutoLayoutNode
{
    AutoLayoutNode *parent;
    AutoLayoutNode *last;
    AutoLayoutNode *sibling;   // Reference to the layout tree sibling.
    AutoLayoutNode *child;     // Reference to the layout tree child.
    AutoLayoutElement element; // Element data.
};

struct AutoLayoutProcess
{
    AutoLayoutNode *root;
    Arena *arena;

    static AutoLayoutProcess init(Arena &arena, AutoLayoutNode **root);
    void deinit();

    AutoLayoutNode *add_element(AutoLayoutNode *parent,
                                AutoLayoutElement element);

    // Returns the count of elements in the destination array.
    void process(Arena &arena, AutoLayoutResult *&result);
};
