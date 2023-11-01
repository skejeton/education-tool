#include "bump_allocator.hpp"
#include <stdlib.h>
#include <cassert>
// TODO: Linked list implementation
#define MAX_ALLOC (1<<16)

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
    return BumpAllocator{ malloc(MAX_ALLOC), 0 };
}

void*
BumpAllocator::alloc(size_t size)
{
    assert(size <= MAX_ALLOC - this->offset &&
      "BumpAllocator: resource exhausted. TODO: implement linked list");

    void* ptr = (char*)this->memory + this->offset;
    this->offset += size;
    return ptr;
}

void
BumpAllocator::deinit()
{
    free(this->memory);
}
