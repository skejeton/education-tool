///
/// BRIEF: Input handling utilities.
///

#pragma once
#include "catedu/core/math/math.hpp"
#include "sokol/sokol_app.h"

struct InputKey
{
    bool pressed, released, held;
    int repeats;
};

#define INPUT_MB_LEFT 512
#define INPUT_MB_RIGHT 513
#define INPUT_MB_MIDDLE 514

#define INPUT_CTRL 515
#define INPUT_SHIFT 516
#define INPUT_ALT 517

#define INPUT_COUNT 518

struct Input
{
    InputKey k[INPUT_COUNT];
    char input[32];
    size_t input_len;
    Vector2 mouse_pos;
    Vector2 mouse_delta;
    int mouse_wheel;
    int last_mouse_wheel;

    Input init();
    void update(); // called every frame
    void clear(int key);
    bool shortcut(int modifier, int key);
    void pass_event(const sapp_event *event);
};
