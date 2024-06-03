#pragma once
#include "catedu/ui/user.hpp"

struct WindowInfo
{
    const char *title;
    Rect rect;
    bool if_static;
};

enum struct MsgBoxType
{
    Info,
    Warning,
    Error
};

bool begin_show_window(UiUser &user, WindowInfo info);
void end_show_window(UiUser &user);

bool input(UiUser &user, const char *id, char *out, int max);

void begin_button_frame(UiUser &user, const char *id, AutoLayoutElement el = {},
                        Vector4 background = {1.0, 1.0, 1.0, 1.0});
bool end_button_frame(UiUser &user);

bool button(UiUser &user, const char *text,
            Vector4 background = {1.0, 1.0, 1.0, 1.0});
bool button_toggle(UiUser &user, const char *text, bool &state);

bool button_radio(UiUser &user, const char *text, int &mode, int val);

void img(UiUser &user, const char *path, Vector2 scale = {1, 1});
void img(UiUser &user, UiImageId id, Vector2 scale = {1, 1});
void label(UiUser &user, const char *text, Vector2 scale = {1, 1},
           UiBrush style = {UiBuffers::Rectangle, {0, 0, 0, 1}, {0, 0, 0, 1}});

/// @returns Which button was pressed. -1 if none.
int msgbox(UiUser &user, const char *title, const char *text, MsgBoxType type,
           const char *buttons[]);
