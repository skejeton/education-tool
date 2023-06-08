#include "help_menu.hpp"
#include "imgui/imgui.h"
#include "sokol/sokol_app.h"

struct HelpEntry {
  char kind;
  char *text;
};

void show_document(HelpEntry *entries, size_t entries_count) {
  ImVec2 cursor = ImGui::GetCursorPos();

  for (size_t i = 0; i < entries_count; i++) {
    HelpEntry entry = entries[i];
    switch (entry.kind) {
      case 'h': 
        ImGui::SetWindowFontScale(1.2);
        ImGui::SeparatorText(entry.text);
        ImGui::SetWindowFontScale(1);
        break;
      case 't':
      default:
        // This is margin
        ImGui::SetCursorPosX(cursor.x+35);
        ImGui::TextWrapped(entry.text);
    }
  }
}

HelpEntry entries[] = {
  // TODO: keep up to date list of ALL shortcuts
  //       make a system to automatically register shortcuts into this document
  {'h', "Keyboard shortcuts"},
  {'t', "[ - Build mode"},
  {'t', "] - Play mode"},

  {'h', "Movement"},
  {'t', "Use WASD keys for movement"},
  
  {'h', "Build mode"},
  {'t', "With build mode you can create your own games. Place buildings and add NPC's to interact with."},

  {'h', "Play mode"},
  {'t', "Play games you made with build mode, interact with NPC's and walk around buildings and trees across pavements."}

};

void HelpMenu::show() {
  if (!this->shown) return;

  const float width = sapp_widthf();

  if (!this->was_init) {
    ImGui::SetNextWindowPos({width-400-10, 400});
    ImGui::SetNextWindowSize({400, 400});
    this->was_init = true;
  }

  if (ImGui::Begin("Help Window", &this->shown, ImGuiWindowFlags_NoCollapse)) {
    show_document(entries, sizeof entries / sizeof entries[0]);
  }
  ImGui::End();
}