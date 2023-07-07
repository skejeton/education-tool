//
// Created by skeje on 7/6/2023.
//

#include "easy_gui.hpp"
#include <assert.h>
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#define WIDGET(rect, name, code) do { \
    Rect rect_internal = (rect); ImGui::GetStyle().WindowRounding = 0; \
    ImGui::SetNextWindowPos({rect_internal.pos.x-2, rect_internal.pos.y-2}); \
    ImGui::SetNextWindowSize({rect_internal.siz.x+4, rect_internal.siz.y+4}); \
    ImGui::Begin(name, NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus); \
    { code } \
    ImGui::End();            \
    } while(0)

void push_layout(EasyGui *easy_gui, Layout::Type type, Rect rect) {
    Layout layout = {};
    layout.type = type;
    layout.rect = rect;

    easy_gui->layouts.push_back(layout);

//    ImGui::GetForegroundDrawList()->AddRect({rect.pos.x, rect.pos.y}, {rect.pos.x+rect.siz.x, rect.pos.y+rect.siz.y}, 0xFF0000FF, 0, 0, 5);
}

Layout *get_last_layout(EasyGui *easy_gui) {
    if (easy_gui->layouts.empty()) {
        return nullptr;
    }
    return &easy_gui->layouts[easy_gui->layouts.size()-1];
}

Rect push_element(EasyGui *easy_gui, Vector2 size) {
    Layout *last_layout = get_last_layout(easy_gui);
    assert(last_layout && "You don't have an existing layout to push your elements");
    Rect result = {};

    size += Vector2{easy_gui->padding, easy_gui->padding};

    switch (last_layout->type) {
        case Layout::COLUMN:
            if (easy_gui->stretch) {
                size.x = last_layout->rect.siz.x-easy_gui->margin*2;
            }
            result.pos = {last_layout->rect.pos + Vector2{easy_gui->margin, last_layout->offset_l+easy_gui->margin}};
            result.siz = size;
            last_layout->offset_l += size.y+easy_gui->margin;
            break;
        case Layout::ROW:
            if (easy_gui->stretch) {
                size.y = last_layout->rect.siz.y-easy_gui->margin*2;
            }
            if (easy_gui->right_to_left) {
                last_layout->offset_r -= size.x;
                result.pos = {last_layout->rect.pos + Vector2{last_layout->offset_r+last_layout->rect.pos.x+last_layout->rect.siz.x-easy_gui->margin, easy_gui->margin}};
                result.siz = size;
                last_layout->offset_r -= easy_gui->margin;
            } else {
                result.pos = {last_layout->rect.pos + Vector2{last_layout->offset_l+easy_gui->margin, easy_gui->margin}};
                result.siz = size;
                last_layout->offset_l += size.x+easy_gui->margin;
            }
            break;
    }

    return result;
}

void EasyGui::end() {
    end_layout();
    assert(layouts.empty() && "Layout overflow");
    ImGui::PopStyleVar();
}

void EasyGui::begin(Vector2 window_size) {
    Rect rect = {0, 0, window_size.x, window_size.y };
    push_layout(this, Layout::Type::COLUMN, rect);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
}

void EasyGui::begin_layout_cut(Layout::Type type, Side side, float size) {
    Layout *last_layout = get_last_layout(this);
    assert(last_layout && "You need to call begin before begin_layout_cut");

    Rect *prev_rect = &last_layout->rect;
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
            curr_rect.pos.x = prev_rect->pos.x+prev_rect->siz.x-size;
            prev_rect->siz.x -= size;
            break;
        case TOP:
            curr_rect.pos.y = prev_rect->pos.y;
            prev_rect->pos.y += size;
            prev_rect->siz.y -= size;
            break;
        case BOTTOM:
            curr_rect.pos.y = prev_rect->pos.y+prev_rect->siz.y-size;
            prev_rect->siz.y -= size;
            break;
    }


    push_layout(this, type, curr_rect);
}

void EasyGui::begin_layout(Layout::Type type) {
    Layout *last_layout = get_last_layout(this);
    assert(last_layout && "You need to call begin before begin_layout");
    push_layout(this, type, last_layout->rect);
}

void EasyGui::end_layout() {
    assert(!layouts.empty() && "Layout underflow");
    layouts.pop_back();
}

bool EasyGui::button(const char *text) {
    ImVec2 text_size = ImGui::CalcTextSize(text);

    Rect rect = push_element(this, Vector2{text_size.x, text_size.y});


    bool clicked = false;
    WIDGET(rect, text, {
        ImGui::SetCursorScreenPos({rect.pos.x, rect.pos.y});
        clicked = ImGui::Button(text, {rect.siz.x, rect.siz.y});
    });

    return clicked;
}

void EasyGui::label(const char *text, ...) {
    ImVec2 text_size = ImGui::CalcTextSize(text);

    Rect rect = push_element(this, Vector2{text_size.x, text_size.y});

    va_list va;
    va_start(va, text);
    char str[512];
    vsnprintf(str, 512, text, va);
    va_end(va);

    ImGui::GetForegroundDrawList()->AddText({rect.pos.x, rect.pos.y}, ImGui::GetColorU32(ImGuiCol_Text), str);
}

void EasyGui::background() {
    Rect rect = get_last_layout(this)->rect;

    ImGui::SetNextWindowPos({rect.pos.x-2, rect.pos.y-2});
    ImGui::SetNextWindowSize({rect.siz.x+4, rect.siz.y+4});
    ImGui::Begin("background", nullptr, ImGuiWindowFlags_NoDecoration  | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::End();
}
