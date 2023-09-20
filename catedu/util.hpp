///
/// BRIEF: Various utilities
///

#ifndef H_CATEDU_UTIL
#define H_CATEDU_UTIL

#include <assert.h>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <vector>


#include "math.hpp"

char*
string_duplicate(const char*);
void
center_next_window(Vector2 size);
void
rect_to_uv(Rect rect, Vector2 texture_size, Vector2* uv0, Vector2* uv1);
std::string
join_vector_into_string(std::vector<std::string>& v, const char* delim);
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
#endif // H_CATEDU_UTIL
