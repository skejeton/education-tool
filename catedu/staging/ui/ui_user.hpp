#pragma once
#include "catedu/core/storage/id_retainer.hpp"
#include "catedu/math.hpp"
#include "catedu/ui/rendering/core.hpp"
#include "catedu/ui/rendering/font.hpp"
#include "catedu/ui/rendering/pass.hpp"
#include "sokol/sokol_app.h"
#include "ui_autolayout.hpp"

struct UiInput
{
    Vector2 mouse_pos;
    Vector2 mouse_delta;
    bool mouse_down;
    bool mouse_pressed;
};

struct UiPersistentElement
{
    Rect border_rect;
};

struct UiState
{
    UiRenderingCore core;
    UiFontRenderer font;
    UiInput input;
    IdRetainer<UiPersistentElement> elements;

    static UiState init(const char* font_path);
    void feed_event(const sapp_event* event);
    void deinit();
};

struct UiGenericStyles
{
    UiBrush brush;
    UiBrush border;
    const char* text;
    Vector2 text_scale;
    UiPersistentElement* persistent;
};

struct UiUser
{
    UiState* state;
    UiRenderingPass pass;
    AutoLayoutProcess layout;
    AutoLayoutNodeId current_node;
    Table<UiGenericStyles> styles;
    BumpAllocator bump;

    static UiUser init(UiState& state);

    void begin_pass();
    void end_pass();

    bool button(const char* text);
    void label(const char* text,
               Vector2 scale = { 1, 1 },
               UiBrush style = { UiBuffers::Rectangle,
                                 { 1, 1, 1, 1 },
                                 { 1, 1, 1, 1 } });
    void begin_generic(AutoLayoutElement el,
                       UiBrush brush,
                       UiBrush border,
                       UiPersistentElement* persistent = nullptr);
    void end_generic();
};
