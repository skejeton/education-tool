#include "allocator.hpp"
#include <cassert>
#include <stdio.h>
#include <stdlib.h>

MallocAllocator ALLOCATOR_MALLOC = {};

void MemoryTracer::alloc(size_t size)
{
    // printf("Allocating %zu bytes\n", size);
    total_allocations++;
    total_bytes_allocated += size;
}

void MemoryTracer::free(size_t size)
{
    total_frees++;
    total_bytes_allocated -= size;
}

void *MallocAllocator::alloc(size_t size)
{
    tracer.alloc(size);
    void *data = malloc(size + sizeof(size_t));
    assert(data);
    *(size_t *)data = size;
    return (char *)data + sizeof(size_t);
}

void *MallocAllocator::realloc(void *ptr, size_t size)
{
    if (!ptr)
        return alloc(size);

    void *data = (char *)ptr - sizeof(size_t);
    size_t old_size = *(size_t *)data;
    tracer.free(old_size);
    tracer.alloc(size);
    void *new_data = ::realloc(data, size + sizeof(size_t));
    assert(new_data);
    *(size_t *)new_data = size;
    return (char *)new_data + sizeof(size_t);
}

void MallocAllocator::free(void *ptr)
{
    if (!ptr)
        return;
    void *data = (char *)ptr - sizeof(size_t);
    tracer.free(*(size_t *)data);
    ::free(data);
}
