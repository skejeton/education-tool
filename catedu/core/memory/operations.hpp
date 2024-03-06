#pragma once
#include "catedu/sys/oom.hpp"
#include <string.h>

template <class T> T *memory_copy_via_malloc(T *elements, size_t count)
{
    return (T *)memcpy(OOM_HANDLER(malloc(count * sizeof(T))), elements,
                       count * sizeof(T));
}
