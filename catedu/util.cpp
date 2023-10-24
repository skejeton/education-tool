#include "util.hpp"
#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "catedu/core/memory/buffer.hpp"
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
