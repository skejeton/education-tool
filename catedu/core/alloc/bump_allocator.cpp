#include "bump_allocator.hpp"
#include <stdlib.h>

void
resize(BumpAllocator* allocator, size_t new_cap)
{
    allocator->memory = realloc(allocator->memory, new_cap);
    allocator->cap = new_cap;
}

void
resize_if_needed(BumpAllocator* allocator, size_t size)
{
    if (allocator->offset + size > allocator->cap) {
        size_t new_cap = allocator->cap * 2;
        resize(allocator, new_cap);
    }
}

void*
allocate_after_we_have_enough_space(BumpAllocator* allocator, size_t size)
{
    void* ptr = (char*)allocator->memory + allocator->offset;
    allocator->offset += size;
    return ptr;
}

BumpAllocator
BumpAllocator::init()
{
    return BumpAllocator{ NULL, 0, 0 };
}

void*
BumpAllocator::alloc(size_t size)
{
    resize_if_needed(this, size);
    return allocate_after_we_have_enough_space(this, size);
}

void
BumpAllocator::deinit()
{
    free(this->memory);
}
