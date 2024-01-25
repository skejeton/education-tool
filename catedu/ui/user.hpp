#pragma once
#include "catedu/core/math/math.hpp"
#include "catedu/core/storage/id_retainer.hpp"
#include "catedu/ui/rendering/core.hpp"
#include "catedu/ui/rendering/font.hpp"
#include "catedu/ui/rendering/pass.hpp"
#include "layout/autolayout.hpp"
#include "sokol/sokol_app.h"

struct UiInput
{
    Vector2 mouse_pos;
    Vector2 mouse_delta;
    bool escape, tab;
    bool mouse_down;
    bool mouse_pressed;
    char inputchars[8];
    int inputchars_count;
};

struct UiPersistentElement
{
    bool focusable;
    int order;
    Rect border_box;
};

struct UiElementStorage
{
    Table<UiPersistentElement> elements;
    IdRetainer<TableId> element_retainer;
    int order;

    void begin_cycle();
    void end_cycle();

    void push(const char *id, UiPersistentElement value = {});
    void pop();
    UiPersistentElement *value();
    TableId id();

    static UiElementStorage init();
    void deinit();
};

struct UiInteractionStateTable
{
    TableId hovered;
    TableId focused;
};

struct UiState
{
    UiRenderingCore *core;
    UiFontRenderer font;
    UiInput input;
    UiElementStorage element_storage;
    UiInteractionStateTable interaction_table;

    float dpi_scale;

    static UiState init(const char *font_path, float dpi_scale = 1);
    void deinit();

    bool feed_event(const sapp_event *event);
};

struct UiInteractionStatePass
{
    UiElementStorage *element_storage;
    UiInteractionStateTable *interaction_table;
    Vector2 mouse_pos;
    bool interaction;
    bool exit_interaction;
    bool switch_interaction; // e.g. tab is pressed

    void process();
};

struct UiGenericStyles
{
    UiBrush brush;
    UiBrush border;
    const char *text;
    Vector2 text_scale;
    TableId persistent;
};

struct UiUser
{
    UiState *state;
    UiRenderingPass pass;
    AutoLayoutProcess layout;
    AutoLayoutNodeId current_node;
    Table<UiGenericStyles> styles;
    BumpAllocator bump;

    static UiUser init(UiState &state);

    void begin_pass();
    void end_pass();

    void input(const char *id, char *out, int max);
    bool button(const char *text, int offs = 0);
    void label(const char *text, Vector2 scale = {1, 1},
               UiBrush style = {
                   UiBuffers::Rectangle, {1, 1, 1, 1}, {1, 1, 1, 1}});
    void label_tooltip(const char *text, const char *tooltip,
                       Vector2 scale = {1, 1},
                       UiBrush style = {
                           UiBuffers::Rectangle, {1, 1, 1, 1}, {1, 1, 1, 1}});

    void begin_generic(AutoLayoutElement el, UiBrush brush, UiBrush border,
                       TableId persistent = NULL_ID);
    void end_generic();
};
