#include "dialog.hpp"
#include "util.hpp"
#include "imgui/imgui.h"

bool Dialog::show()
{
    center_next_window({300, 250});
    switch (this->type) {
    case DialogType::Info: ImGui::Begin("Info"); break;
    case DialogType::Pending: ImGui::Begin("Pending"); break;
    case DialogType::Warning: ImGui::Begin("Warning"); break;
    case DialogType::Error: ImGui::Begin("Error"); break;
    }

    ImGui::Text("%s", this->text.c_str());

    bool pressed = ImGui::Button("Ok");
  
    ImGui::End();

    return pressed;
}


void DialogQueue::push(std::string text, DialogType type)
{
    this->dialogs.push({type, text});
}


void DialogQueue::show()
{
    if (!this->dialogs.empty()) {
        if (this->dialogs.front().show()) {
            this->dialogs.pop();
        }
    }
}
