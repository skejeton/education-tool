#ifndef HPP_CATEDU_UI_AUTOLAYOUT
#define HPP_CATEDU_UI_AUTOLAYOUT

#include "catedu/core/alloc/bump_allocator.hpp"
#include "catedu/math.hpp"
#include "catedu/table.hpp"

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
        Column,
        Row
    };

    Type type;
};

struct AutoLayoutDimension
{
    enum Type
    {
        Auto,
        Pixel
    };

    Type type;
    float value;
};

struct AutoLayoutElement
{
    TableId userdata; // Reference to userdata.
    AutoLayoutDimension width;
    AutoLayoutDimension height;
    float align_width;
    float align_height;
    TRBL margin;
    TRBL border;
    TRBL padding;
    AutoLayout layout; // Layout-specific data.
    char *note;        // Note for debugging.
    //----------------------------------------------- Calculated data.
    Rect padding_box; // Box inside padding.
    Rect base_box;    // Box inside border.
    Rect border_box;  // Box inside margin.
    Rect margin_box;  //
};

struct AutoLayoutResult
{
    TableId userdata;
    Rect padding_box; // Box inside padding.
    Rect base_box;    // Box inside border.
    Rect border_box;  // Box inside margin.
    Rect margin_box;  //
    char *note;
    AutoLayoutResult *next;
};

struct AutoLayoutNodeId
{
    TableId id;
};

// Different from the `AutoLayoutElement` because it contains references to
// other nodes.
struct AutoLayoutNode
{
    AutoLayoutNodeId parent;
    AutoLayoutNodeId last;
    AutoLayoutNodeId sibling;  // Reference to the layout tree sibling.
    AutoLayoutNodeId child;    // Reference to the layout tree child.
    AutoLayoutElement element; // Element data.
};

struct AutoLayoutProcess
{
    Table<AutoLayoutNode> nodes;
    AutoLayoutNodeId root;
    BumpAllocator notes;

    static AutoLayoutProcess init(AutoLayoutNodeId &root);
    void deinit();

    AutoLayoutNodeId add_element(AutoLayoutNodeId parent,
                                 AutoLayoutElement element);

    // Returns the count of elements in the destination array.
    void process(BumpAllocator alloc, AutoLayoutResult *&result);
};

#endif
