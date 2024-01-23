#pragma once
#include "catedu/ui/rendering/make_brush.hpp"
#include "catedu/ui/user.hpp"

struct WindowInfo
{
    const char *title;
    Rect rect;
};

void begin_show_window(UiUser &user, WindowInfo info);
void end_show_window(UiUser &user);
