///
/// BRIEF: Input handling utilities.
///

#ifndef H_INPUT_CATEDU
#define H_INPUT_CATEDU

#include "sokol/sokol_app.h"
#include "math.hpp"

struct InputKey {
    bool pressed, released, held;
};

struct Input {
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

#endif