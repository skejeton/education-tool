//
// Created by skeje on 7/6/2023.
//

#include "easy_gui.hpp"
#include <assert.h>
#include "imgui/imgui.h"

void push_layout(EasyGui *easy_gui, Layout::Type type, Rect rect) {
    Layout layout = {};
    layout.type = type;
    layout.rect = rect;

    easy_gui->layouts.push_back(layout);
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

    switch (last_layout->type) {
        case Layout::COLUMN:
            result.pos = {last_layout->rect.pos + Vector2{0, last_layout->offset_l}};
            result.siz = size;
            last_layout->offset_l += size.y+easy_gui->margin;
            break;
        case Layout::ROW:
            if (easy_gui->right_to_left) {
                last_layout->offset_r -= size.x;
                result.pos = {last_layout->rect.pos + Vector2{last_layout->offset_r+last_layout->rect.pos.x+last_layout->rect.siz.x, 0}};
                result.siz = size;
                last_layout->offset_r -= easy_gui->margin;
            } else {
                result.pos = {last_layout->rect.pos + Vector2{last_layout->offset_l, 0}};
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
    ImGui::End();
    ImGui::PopStyleVar();
}

void EasyGui::begin(Vector2 window_size) {
    Rect rect = {0, 0, window_size.x, window_size.y };
    push_layout(this, Layout::Type::COLUMN, rect);

    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize({0, 0});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
    ImGui::Begin("Main", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
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

    Rect button_rect = push_element(this, Vector2{text_size.x, text_size.y} + Vector2{padding, padding});

    ImGui::SetCursorScreenPos({button_rect.pos.x, button_rect.pos.y});
    return ImGui::Button(text, {button_rect.siz.x, button_rect.siz.y});
}
