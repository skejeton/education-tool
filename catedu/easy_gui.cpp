//
// Created by skeje on 7/6/2023.
//

#include "easy_gui.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include <assert.h>

#define WIDGET(easy_gui, rect, name, code)                                     \
    do {                                                                       \
        EasyGui* eg_ptr__ = easy_gui;                                          \
        eg_ptr__->tricks.begin_widget(rect, name);                             \
        { code } eg_ptr__->tricks.end_widget();                                \
    } while (0)

void
push_layout(EasyGui* easy_gui, Layout::Type type, Rect rect, bool push)
{
    Layout layout = {};
    layout.type = type;
    layout.rect = rect;
    layout.push = push;

    easy_gui->layouts.push_back(layout);
}

Layout*
get_last_layout(EasyGui* easy_gui)
{
    if (easy_gui->layouts.empty()) {
        return nullptr;
    }
    return &easy_gui->layouts[easy_gui->layouts.size() - 1];
}

Rect
push_element(EasyGui* easy_gui, Vector2 size)
{
    Layout* last_layout = get_last_layout(easy_gui);
    assert(last_layout &&
           "You don't have an existing layout to push your elements");
    Rect result = {};

    size += Vector2{ easy_gui->padding, easy_gui->padding };

    switch (last_layout->type) {
        case Layout::COLUMN:
            if (easy_gui->stretch) {
                size.x = last_layout->rect.siz.x - easy_gui->margin * 2;
            }
            result.pos = { last_layout->rect.pos +
                           Vector2{ easy_gui->margin,
                                    last_layout->offset_l +
                                      easy_gui->margin } };
            result.siz = size;
            last_layout->offset_l += size.y + easy_gui->margin;
            last_layout->secn_size =
              std::max(last_layout->secn_size, result.siz.x);
            break;
        case Layout::ROW:
            if (easy_gui->stretch) {
                size.y = last_layout->rect.siz.y - easy_gui->margin * 2;
            }
            if (easy_gui->right_to_left) {
                last_layout->offset_r -= size.x;
                result.pos = {
                    last_layout->rect.pos +
                    Vector2{ last_layout->offset_r + last_layout->rect.pos.x +
                               last_layout->rect.siz.x - easy_gui->margin,
                             easy_gui->margin }
                };
                result.siz = size;
                last_layout->offset_r -= easy_gui->margin;
            } else {
                result.pos = { last_layout->rect.pos +
                               Vector2{ last_layout->offset_l +
                                          easy_gui->margin,
                                        easy_gui->margin } };
                result.siz = size;
                last_layout->offset_l += size.x + easy_gui->margin;
            }
            last_layout->secn_size =
              std::max(last_layout->secn_size, result.siz.y);
            break;
    }

    //    ImGui::GetForegroundDrawList()->AddRect({result.pos.x, result.pos.y},
    //    {result.pos.x+result.siz.x, result.pos.y+result.siz.y}, 0xFF5500FF, 0,
    //    0, 1);

    return result;
}

Vector2
calc_layout_content_size(Layout* layout)
{
    float prim_size = layout->offset_l;
    float secn_size = layout->secn_size;

    if (layout->offset_r < 0) {
        prim_size = layout->rect.siz.x;
    }

    switch (layout->type) {
        case Layout::ROW:
            return { prim_size, secn_size };
        case Layout::COLUMN:
            return { secn_size, prim_size };
    }
    assert(false);
    return {};
}

void
EasyGui::end()
{
    tricks.check_end();
    end_layout();
    assert(layouts.empty() && "Layout overflow");
    ImGui::PopStyleVar();
}

void
EasyGui::begin(Vector2 window_size)
{
    Rect rect = { 0, 0, window_size.x, window_size.y };
    push_layout(this, Layout::Type::COLUMN, rect, false);
    this->static_counter = 0;
    this->font_scale = 1;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
}

void
EasyGui::begin_layout_cut(Layout::Type type, Side side, float size)
{
    Layout* last_layout = get_last_layout(this);
    assert(last_layout && "You need to call begin before begin_layout_cut");

    Rect* prev_rect = &last_layout->rect;
    Rect curr_rect = {};
    switch (side) {
        case LEFT:
        case RIGHT:
            curr_rect.pos.y = prev_rect->pos.y;
            curr_rect.siz.y = prev_rect->siz.y;
            curr_rect.siz.x = size;
            break;
        case TOP:
        case BOTTOM:
            curr_rect.pos.x = prev_rect->pos.x;
            curr_rect.siz.x = prev_rect->siz.x;
            curr_rect.siz.y = size;
            break;
    }
    switch (side) {
        case LEFT:
            curr_rect.pos.x = prev_rect->pos.x;
            prev_rect->pos.x += size;
            prev_rect->siz.x -= size;
            break;
        case RIGHT:
            curr_rect.pos.x = prev_rect->pos.x + prev_rect->siz.x - size;
            prev_rect->siz.x -= size;
            break;
        case TOP:
            curr_rect.pos.y = prev_rect->pos.y;
            prev_rect->pos.y += size;
            prev_rect->siz.y -= size;
            break;
        case BOTTOM:
            curr_rect.pos.y = prev_rect->pos.y + prev_rect->siz.y - size;
            prev_rect->siz.y -= size;
            break;
    }

    push_layout(this, type, curr_rect, false);
}

void
EasyGui::begin_layout(Layout::Type type)
{
    Layout* last_layout = get_last_layout(this);
    assert(last_layout && "You need to call begin before begin_layout");

    switch (last_layout->type) {
        case Layout::COLUMN:
            push_layout(this,
                        type,
                        { last_layout->rect.pos.x,
                          last_layout->rect.pos.y + last_layout->offset_l,
                          last_layout->rect.siz.x,
                          0 },
                        true);
            break;
        case Layout::ROW:
            push_layout(this,
                        type,
                        { last_layout->rect.pos.x + last_layout->offset_l,
                          last_layout->rect.pos.y,
                          0,
                          last_layout->rect.siz.y },
                        true);
            break;
    }
}

void
EasyGui::end_layout()
{
    assert(!layouts.empty() && "Layout underflow");
    Layout last_layout = *get_last_layout(this);
    layouts.pop_back();
    if (last_layout.push) {
        push_element(this, calc_layout_content_size(&last_layout));
    }
}

bool
EasyGui::button(const char* text)
{
    ImVec2 text_size = ImGui::CalcTextSize(text);
    text_size.x *= font_scale;
    text_size.y *= font_scale;

    Rect rect = push_element(this, Vector2{ text_size.x, text_size.y });

    bool clicked = false;
    WIDGET(this, rect, text, {
        ImGui::SetWindowFontScale(font_scale);
        ImGui::SetCursorScreenPos({ rect.pos.x, rect.pos.y });
        clicked = ImGui::Button(text, { rect.siz.x, rect.siz.y });
    });

    return clicked;
}

void
EasyGui::label(const char* text, ...)
{
    char buf[512];

    va_list va;
    Layout* last_layout = get_last_layout(this);

    va_start(va, text);
    vsnprintf(buf, 512, text, va);
    va_end(va);

    ImVec2 text_size = ImGui::CalcTextSize(buf);
    text_size.x *= font_scale;
    text_size.y *= font_scale;

    // NOTE: Wrapping workaround
    if (last_layout->type == Layout::COLUMN) {
        float parent_width = last_layout->rect.siz.x;
        float increment = text_size.y;
        float width = text_size.x;

        while (width > parent_width) {
            text_size.y += increment;
            width -= parent_width;
        }
    }

    Rect rect = push_element(this, Vector2{ text_size.x, text_size.y });

    char id[32];
    snprintf(id, 32, "LABEL%d", static_counter++);

    WIDGET(this, rect, id, {
        ImGui::SetWindowFontScale(font_scale);
        ImGui::SetCursorScreenPos({ rect.pos.x, rect.pos.y });
        ImGui::TextWrapped("%s", buf);
    });

    static_counter++;
}

void
EasyGui::input_text_multiline(const char* name, char* buf, size_t max)
{
    ImVec2 element_size = { 300, 400 };

    Rect rect = push_element(this, Vector2{ element_size.x, element_size.y });

    WIDGET(this, rect, name, {
        ImGui::SetWindowFontScale(font_scale);
        ImGui::SetCursorScreenPos({ rect.pos.x, rect.pos.y });
        ImGui::InputTextMultiline(name, buf, max, { rect.siz.x, rect.siz.y });
    });
}

void
EasyGui::input_text(const char* name, char* buf, size_t max)
{
    ImVec2 element_size = { 100, ImGui::CalcTextSize(" ").y };

    Rect rect = push_element(this, Vector2{ element_size.x, element_size.y });

    WIDGET(this, rect, name, {
        ImGui::SetWindowFontScale(font_scale);
        ImGui::SetCursorScreenPos({ rect.pos.x, rect.pos.y });
        ImGui::InputTextEx(name, "", buf, max, { rect.siz.x, rect.siz.y }, 0);
    });
}

void
EasyGui::checkbox(const char* name, bool* value)
{
    ImVec2 element_size = { 300, ImGui::CalcTextSize(" ").y };

    Rect rect = push_element(this, Vector2{ element_size.x, element_size.y });

    WIDGET(this, rect, name, {
        ImGui::SetWindowFontScale(font_scale);
        ImGui::SetCursorScreenPos({ rect.pos.x, rect.pos.y });
        ImGui::Checkbox(name, value);
    });
}

void
EasyGui::push_id(int id)
{
    tricks.push_id(id);
}

void
EasyGui::pop_id()
{
    tricks.pop_id();
}

void
EasyGui::begin_window(const char* name)
{
    Layout* last_layout = get_last_layout(this);
    last_layout->rect = this->tricks.begin_window(last_layout->rect, name);
}

void
EasyGui::end_window()
{
    this->tricks.end_widget();
}
