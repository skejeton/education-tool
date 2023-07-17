//
// Created by skeje on 7/14/2023.
//

#include <cstddef>
#include "imgui_tricks.hpp"


/// @returns Content rect
static Rect begin_general_window(ImGuiTricks *tricks, Rect rect, const char *name, ImGuiWindowFlags flags)
{
    char id[512];
    if (!tricks->id_stack.empty()) {
        snprintf(id, 512, "%s##tricks%d", name, tricks->id_stack[0]);
    } else {
        snprintf(id, 512, "%s##tricks ", name);
    }

    ImGui::GetStyle().WindowRounding = 0;

    if (!tricks->window_nesting) {
        ImGui::SetNextWindowPos({rect.pos.x-2, rect.pos.y-2});
        ImGui::SetNextWindowSize({rect.siz.x+4, rect.siz.y+4});

        ImGui::Begin(id, nullptr, flags);
    }
    tricks->window_nesting++;

    for (auto push_id : tricks->id_stack) {
        ImGui::PushID(push_id);
    }

    ImVec2 content_min = ImGui::GetWindowContentRegionMin();
    ImVec2 content_max = ImGui::GetWindowContentRegionMax();
    ImVec2 window_pos = ImGui::GetWindowPos();

    content_min.x += window_pos.x;
    content_min.y += window_pos.y;
    content_max.x += window_pos.x;
    content_max.y += window_pos.y;

    return rect_from_min_max({content_min.x, content_min.y}, {content_max.x, content_max.y});
}


Rect ImGuiTricks::begin_window(Rect rect, const char *title) {
    return begin_general_window(this, rect, title, ImGuiWindowFlags_None);
}


void ImGuiTricks::begin_widget(Rect rect, const char *id) {
    begin_general_window(this, rect, id, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
}

void ImGuiTricks::end_widget() {
    for (auto push_id : this->id_stack) {
        ImGui::PopID();
    }

    assert(this->window_nesting && "begin/end mismatch");
    this->window_nesting--;

    if (!this->window_nesting) {
        ImGui::End();
    }
}

void ImGuiTricks::check_end() {
    assert(this->window_nesting == 0 && "Unfinished begin call");
    assert(this->id_stack.empty() && "Unfinished id stack");
}

void ImGuiTricks::push_id(int value) {
    this->id_stack.push_back(value);
}

void ImGuiTricks::pop_id() {
    assert(!this->id_stack.empty() && "push/pop mismatch");
    this->id_stack.pop_back();
}

