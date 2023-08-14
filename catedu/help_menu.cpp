#include "help_menu.hpp"
#include "imgui/imgui.h"
#include "sokol/sokol_app.h"

struct HelpEntry {
    char kind;
    const char *text;
};

void show_document(EasyGui *gui, HelpEntry *entries, size_t entries_count) {
    gui->padding = 0;
    for (size_t i = 0; i < entries_count; i++) {
        HelpEntry entry = entries[i];
        switch (entry.kind) {
            case 'h':
                gui->margin = 5;
                gui->font_scale = 1.2;
                gui->label("%s", entry.text);
                gui->font_scale = 1;
                break;
            case 't':
            default:
                gui->margin = 10;
                // This is margin
                ImGui::PushStyleColor(ImGuiCol_Text, 0x77FFFFFF);
                gui->label("%s", entry.text);
                ImGui::PopStyleColor();
                break;
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

void HelpMenu::show(EasyGui *gui) {
    show_document(gui, entries, sizeof entries / sizeof entries[0]);
}