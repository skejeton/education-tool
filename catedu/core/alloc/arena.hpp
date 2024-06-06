///
/// Arena allocator for fast memory allocation,
///

#pragma once

#include "allocator.hpp"
#include <cstdint>

struct Arena
{
    static const constexpr size_t ALIGN = 16;

    struct Chunk
    {
        Chunk *next;
        size_t size;
        size_t cap;
        uint8_t data[];
    };

    Allocator *allocator;
    Chunk *first;
    Chunk *last;
    size_t next_size;

    static Arena create(Allocator *allocator);
    void destroy();
    void reset();

    void *alloc(size_t size);

    template <class T> T *alloct()
    {
        return (T *)this->alloc(sizeof(T));
    }
};
