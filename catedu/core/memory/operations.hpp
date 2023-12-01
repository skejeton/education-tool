#ifndef HPP_CATEDU_CORE_MEMORY_BUFFER
#define HPP_CATEDU_CORE_MEMORY_BUFFER

#include <string.h>

template <class T> T *memory_copy_via_malloc(T *elements, size_t count)
{
    return (T *)memcpy(malloc(count * sizeof(T)), elements, count * sizeof(T));
}

#endif
