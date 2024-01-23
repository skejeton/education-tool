#pragma once
#include <stddef.h>

// TODO: Allocator chaining, for now using `malloc`
// This is a one-way allocator, it can only allocate memory, not free it.
struct BumpAllocator
{
    void *memory;
    size_t offset;

    static BumpAllocator init();
    void *alloc(size_t size);

    template <class T> T *alloc()
    {
        return (T *)this->alloc(sizeof(T));
    }

    // @warning All data allocated with this allocator will be lost.
    void deinit();
};
