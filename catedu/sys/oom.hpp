#pragma once

#include <assert.h>

#define OOM_HANDLER(v)                                                         \
    [](void *ptr) {                                                            \
        assert(ptr != nullptr && "Out of memory.");                            \
        return ptr;                                                            \
    }(v)
