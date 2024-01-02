///
/// BRIEF: Input handling utilities.
///

#pragma once
#include "math.hpp"
#include "sokol/sokol_app.h"

struct InputKey
{
    bool pressed, released, held;
};

struct Input
{
    InputKey key_states[512];
    InputKey mouse_states[3];
    Vector2 mouse_pos;
    Vector2 mouse_delta;
    int mouse_wheel;
    int last_mouse_wheel;

    Input init();
    void update(); // called every frame
    void pass_event(const sapp_event *event);
};
