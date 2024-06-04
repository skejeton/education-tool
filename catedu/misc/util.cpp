#include "util.hpp"
#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

void rect_to_uv(Rect rect, Vector2 texture_size, Vector2 *uv0, Vector2 *uv1)
{
    *uv0 = {rect.pos.x / texture_size.x, rect.pos.y / texture_size.y};
    *uv1 = {(rect.pos.x + rect.siz.x) / texture_size.x,
            (rect.pos.y + rect.siz.y) / texture_size.y};
}

std::string join_vector_into_string(std::vector<std::string> &v,
                                    const char *delim)
{
    std::string result;
    for (size_t i = 0; i < v.size(); i++)
    {
        result += v[i];
        if (i != v.size() - 1)
        {
            result += delim;
        }
    }
    return result;
}
