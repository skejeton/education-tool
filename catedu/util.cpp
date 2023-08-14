#include "util.hpp"
#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "sokol/sokol_app.h"
#include "imgui/imgui.h"

// Returns a malloc-allocated string 
char *string_duplicate(const char *string) {
    if (string == nullptr) {
        return nullptr;
    }
    size_t count = strlen(string)+1;
    char *new_string = (char*)malloc(count);

    memcpy(new_string, string, count);
    return new_string;
}


void center_next_window(Vector2 size)
{
    ImGui::SetNextWindowPos({(sapp_widthf()-size.x)/2.0f, (sapp_heightf()-size.y)/2.0f});
    ImGui::SetNextWindowSize({size.x, size.y});
}