#pragma once
#include "catedu/math.hpp"
#include "catedu/ui/rendering/core.hpp"
#include "catedu/ui/rendering/font.hpp"
#include "catedu/ui/rendering/pass.hpp"
#include "sokol/sokol_app.h"

struct UiInput
{
    Vector2 mouse_pos;
    Vector2 mouse_delta;
    bool mouse_down;
    bool mouse_pressed;
};

struct UiState
{
    UiRenderingCore core;
    UiFontRenderer font;
    UiInput input;

    static UiState init(const char* font_path);
    void feed_event(const sapp_event* event);
};

struct UiUser
{
    UiState* state;
    UiRenderingPass pass;
    float y = 0;

    static UiUser init(UiState& state);

    void begin_pass();
    void end_pass();

    bool button(const char* text);
    void label(const char* text);
    void generic(Vector2 size,
                 UiBrush brush,
                 float border_width = 0,
                 UiBrush border = {});
};
