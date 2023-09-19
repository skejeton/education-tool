#include "util.hpp"
#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "catedu/core/memory/buffer.hpp"
#include "imgui/imgui.h"
#include "sokol/sokol_app.h"
#include <stdio.h>
#include <string>
#include <vector>

// Returns a malloc-allocated string
char*
string_duplicate(const char* string)
{
    if (string == nullptr) {
        return nullptr;
    }
    size_t count = strlen(string) + 1;
    char* new_string = (char*)malloc(count);

    memcpy(new_string, string, count);
    return new_string;
}

void
center_next_window(Vector2 size)
{
    ImGui::SetNextWindowPos(
      { (sapp_widthf() - size.x) / 2.0f, (sapp_heightf() - size.y) / 2.0f });
    ImGui::SetNextWindowSize({ size.x, size.y });
}

void
rect_to_uv(Rect rect, Vector2 texture_size, Vector2* uv0, Vector2* uv1)
{
    *uv0 = { rect.pos.x / texture_size.x, rect.pos.y / texture_size.y };
    *uv1 = { (rect.pos.x + rect.siz.x) / texture_size.x,
             (rect.pos.y + rect.siz.y) / texture_size.y };
}

std::string
join_vector_into_string(std::vector<std::string>& v, const char* delim)
{
    std::string result;
    for (size_t i = 0; i < v.size(); i++) {
        result += v[i];
        if (i != v.size() - 1) {
            result += delim;
        }
    }
    return result;
}

template<typename... Args>
std::string
stdstrfmt(const std::string& format, Args... args)
{
    int size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
    assert(size > 0 && "stdstrfmt: invalid format string");
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1);
}
