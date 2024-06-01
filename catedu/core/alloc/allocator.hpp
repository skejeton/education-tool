#pragma once
#include <stddef.h>

struct MemoryTracer
{
    size_t total_allocations = 0;
    size_t total_frees = 0;
    size_t total_bytes_allocated = 0;

    void alloc(size_t size);
    void free(size_t size);
};

struct Allocator
{
    MemoryTracer tracer;
    virtual void *alloc(size_t size) = 0;
    virtual void *realloc(void *ptr, size_t size) = 0;
    virtual void free(void *ptr) = 0;
};

struct MallocAllocator : Allocator
{
    void *alloc(size_t size) override;
    void *realloc(void *ptr, size_t size) override;
    void free(void *ptr) override;
};

extern MallocAllocator ALLOCATOR_MALLOC;
