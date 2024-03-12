#include "input.hpp"

#define DOWN true
#define UP false

static void key_on(InputKey &key, bool down)
{
    if (down)
    {
        key.pressed = true;
        key.held = true;
        key.released = false;
    }
    else
    {
        key.pressed = false;
        key.held = false;
        key.released = true;
    }
}

static void handle_key_event(Input &input, int key_code, bool down)
{
    key_on(input.k[key_code], down);
    switch (key_code)
    {
    case SAPP_KEYCODE_LEFT_CONTROL:
        key_on(input.k[INPUT_CTRL], down);
        break;
    case SAPP_KEYCODE_LEFT_SHIFT:
        key_on(input.k[INPUT_SHIFT], down);
        break;
    case SAPP_KEYCODE_LEFT_ALT:
        key_on(input.k[INPUT_ALT], down);
        break;
    default:
        break;
    }
}

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
    for (int i = 0; i < sizeof(Input::k) / sizeof(InputKey); ++i)
    {
        this->k[i].pressed = false;
        this->k[i].released = false;
    }
}

void Input::pass_event(const sapp_event *event)
{
    size_t mouse_button = INPUT_MB_LEFT + event->mouse_button;

    switch (event->type)
    {
    case SAPP_EVENTTYPE_KEY_UP:
    case SAPP_EVENTTYPE_KEY_DOWN:
        handle_key_event(*this, event->key_code,
                         event->type == SAPP_EVENTTYPE_KEY_DOWN);
        break;
    case SAPP_EVENTTYPE_MOUSE_UP:
    case SAPP_EVENTTYPE_MOUSE_DOWN:
        if (event->mouse_button >= 0 && event->mouse_button < 3)
            key_on(this->k[mouse_button],
                   event->type == SAPP_EVENTTYPE_MOUSE_DOWN);
        break;
    case SAPP_EVENTTYPE_MOUSE_MOVE:
        this->mouse_pos = {event->mouse_x, event->mouse_y};
        this->mouse_delta += {event->mouse_dx, event->mouse_dy};
        break;
    case SAPP_EVENTTYPE_MOUSE_SCROLL:
        this->mouse_wheel =
            event->scroll_y == 0 ? 0 : (event->scroll_y < 0 ? -1 : 1);
        break;
    case SAPP_EVENTTYPE_CHAR:
        if (event->char_code >= 32 && event->char_code < 127 &&
            this->input_len < 31)
        {
            this->input[this->input_len++] = event->char_code;
            this->input[this->input_len] = 0;
        }
        break;
    default:
        break;
    }
}

void Input::clear(int key)
{
    this->k[key].pressed = false;
    this->k[key].released = false;
}

bool Input::shortcut(int modifier, int key)
{
    bool valid = this->k[modifier].held && this->k[key].pressed;
    this->clear(key);
    return valid;
}
