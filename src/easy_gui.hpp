//
// Created by skeje on 7/6/2023.
//

#ifndef H_CATEDU_EASY_GUI
#define H_CATEDU_EASY_GUI

#include "math.hpp"
#include <vector>

struct Layout {
    enum Type {
        COLUMN,
        ROW
    } type;

    Rect rect;
    float offset_l;
    float offset_r;
};

struct EasyGui {
    std::vector<Layout> layouts;
    float margin;
    float padding;
    bool right_to_left;

    void begin(Vector2 window_size);
    void end();

    void begin_layout(Layout::Type type);
    void end_layout();

    bool button(const char *text);
};

#endif // H_CATEDU_EASY_GUI