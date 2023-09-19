///
/// BRIEF: Various utilities
///

#ifndef H_CATEDU_UTIL
#define H_CATEDU_UTIL

#include <cstdlib>
#include <cstring>
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

#endif // H_CATEDU_UTIL
