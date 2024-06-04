///
/// BRIEF: Various utilities
///

#pragma once
#include <assert.h>
#include <string>
#include <vector>

#include "catedu/core/math/math.hpp"

void rect_to_uv(Rect rect, Vector2 texture_size, Vector2 *uv0, Vector2 *uv1);
std::string join_vector_into_string(std::vector<std::string> &v,
                                    const char *delim);
