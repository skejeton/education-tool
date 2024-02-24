#pragma once
#include "catedu/ui/rendering/make_brush.hpp"
#include "catedu/ui/user.hpp"

struct WindowInfo
{
    const char *title;
    Rect rect;
    bool if_static;
};

void begin_show_window(UiUser &user, WindowInfo info);
void end_show_window(UiUser &user);

void input(UiUser &user, const char *id, char *out, int max);
bool button(UiUser &user, const char *text, int offs = 0);
void img(UiUser &user, const char *path, Vector2 scale = {1, 1});
void label(UiUser &user, const char *text, Vector2 scale = {1, 1},
           UiBrush style = {UiBuffers::Rectangle, {0, 0, 0, 1}, {0, 0, 0, 1}});
