//
// Created by skeje on 7/6/2023.
//

#ifndef H_CATEDU_EASY_GUI
#define H_CATEDU_EASY_GUI

#include "imgui_tricks.hpp"
#include "math.hpp"
#include <vector>

struct Layout
{
    enum Type
    {
        COLUMN,
        ROW
    } type;

    bool push;
    Rect rect;
    float offset_l;
    float offset_r;
    float secn_size;
};

enum Side
{
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
};

struct EasyGui
{
    ImGuiTricks tricks;
    std::vector<Layout> layouts;
    // Because we have to place each ImGUI element into a separate window, we
    // need to make a counter for static elements like labels, so their ID's
    // don't collide with each other.
    int static_counter;
    float margin;
    float padding;
    float font_scale;
    bool right_to_left;
    bool stretch;

    void begin(Vector2 window_size);
    void end();

    void begin_layout_cut(Layout::Type type, Side side, float size);
    void begin_layout(Layout::Type type);
    void end_layout();

    bool button(const char* text);
    void input_text_multiline(const char* name, char* buf, size_t max);
    void input_text(const char* name, char* buf, size_t max);
    void checkbox(const char* name, bool* value);
    void label(const char* text, ...);

    void begin_window(const char* name);
    void end_window();

    void push_id(int id);
    void pop_id();
};

#endif // H_CATEDU_EASY_GUI
