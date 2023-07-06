#include "input.hpp"
#include <cstdio>

Input Input::init()
{
    return Input{};
}

void Input::update()
{
    // Reset deltas
    this->mouse_delta = {0, 0};
    this->mouse_wheel = 0;

    // Reset one-frame input states
    for (int i = 0; i < 512; ++i) {
        this->key_states[i].pressed = false;
        this->key_states[i].released = false;
    }

    for (int i = 0; i < 3; ++i) {
        this->mouse_states[i].pressed = false;
        this->mouse_states[i].released = false;
    }
}

void Input::pass_event(const sapp_event *event)
{
    switch (event->type) {
        case SAPP_EVENTTYPE_MOUSE_SCROLL:
            this->mouse_wheel = event->scroll_y == 0 ? 0 : (event->scroll_y < 0 ? -1 : 1);
            break;
        case SAPP_EVENTTYPE_KEY_DOWN:
            this->key_states[event->key_code].held = true;
            this->key_states[event->key_code].pressed = true;
            break;
        case SAPP_EVENTTYPE_KEY_UP:
            this->key_states[event->key_code].held = false;
            this->key_states[event->key_code].pressed = false;
            this->key_states[event->key_code].released = true;
            break;
        case SAPP_EVENTTYPE_MOUSE_MOVE:
            this->mouse_pos = {event->mouse_x, event->mouse_y};
            this->mouse_delta += {event->mouse_dx, event->mouse_dy};
            break;
        case SAPP_EVENTTYPE_MOUSE_DOWN:
            if (event->mouse_button >= 0 && event->mouse_button < 3) {
                this->mouse_states[event->mouse_button].held = true;
                this->mouse_states[event->mouse_button].pressed = true;
            }
            break;
        case SAPP_EVENTTYPE_MOUSE_UP:
            if (event->mouse_button >= 0 && event->mouse_button < 3) {
                this->mouse_states[event->mouse_button].held = false;
                this->mouse_states[event->mouse_button].pressed = false;
                this->mouse_states[event->mouse_button].released = true;
            }
            break;
        default:
            break;
    }
}