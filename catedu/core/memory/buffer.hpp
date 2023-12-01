#ifndef HPP_CATEDU_CORE_MEMORY_BUFFER
#define HPP_CATEDU_CORE_MEMORY_BUFFER

#include <stddef.h>

struct Buffer
{
    size_t size;
    void *data;
};

#endif
