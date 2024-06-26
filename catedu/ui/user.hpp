#pragma once
#include "catedu/core/alloc/arena.hpp"
#include "catedu/core/math/math.hpp"
#include "catedu/core/storage/id_retainer.hpp"
#include "catedu/sys/input.hpp"
#include "catedu/ui/rendering/core.hpp"
#include "catedu/ui/rendering/font.hpp"
#include "catedu/ui/rendering/pass.hpp"
#include "layout/autolayout.hpp"
#include "sokol/sokol_app.h"

struct UiPersistentElement
{
    bool focusable;
    int order;
    Rect border_box;
    Rect persistent_box;
    Vector2 pin;
    bool pinned;
    bool hidden;
    float scale;
    Vector2 scroll;
    void *userdata;
    bool pop;
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
    TableId active;
    TableId focused;
};

struct UiImageCache
{
    char *path;
    UiImageId id;
};

struct UiState
{
    UiRenderingCore *core;
    UiFontRenderer font;
    UiFontRenderer font_bold;
    Input input;
    UiElementStorage element_storage;
    UiInteractionStateTable interaction_table;
    Table<UiImageCache> image_cache;
    Arena lifetime_storage;
    Arena frame_storage;

    float dpi_scale;

    UiImageId get_image(const char *path);

    static UiState init(const char *font_path, const char *font_bold_path,
                        float dpi_scale = 1);
    void deinit();

    bool feed_event(const sapp_event *event);
};

struct UiInteractionStatePass
{
    UiElementStorage *element_storage;
    UiInteractionStateTable *interaction_table;
    Vector2 mouse_pos;
    bool interaction;
    bool active_interaction;
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
    bool bold;
    float scale;
    bool has_filter;
    UiFilter filter;
};

struct UiPass
{
    UiState *state;
    UiRenderingPass pass;
    AutoLayoutProcess layout;
    AutoLayoutNode *current_node;
    bool bold;

    static UiPass init(UiState &state);

    void begin_pass();
    void end_pass();

    void push_id(int64_t id);
    void push_id(const char *id);
    void pop_id();

    bool focused();
    bool active();
    bool actively_hovered();
    bool hovered();

    void begin_generic(AutoLayoutElement el, UiBrush brush, UiBrush border,
                       TableId persistent = NULL_ID, float scale = 1.0,
                       bool has_filter = false,
                       UiFilter filter = {0xFFFFFFFF, 0xFFFFFFFF});
    void end_generic();
};
