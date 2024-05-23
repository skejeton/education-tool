///
/// Arena allocator for fast memory allocation,
///

#pragma once

struct Arena
{
    static const constexpr size_t ALIGN = 16;

    struct Chunk
    {
        Chunk *next;
        size_t size;
        size_t cap;
        char data[];
    };

    Chunk *first;
    Chunk *last;
    size_t next_size;

    static Arena create_malloc();
    void destroy();
    void reset();

    void *alloc(size_t size);

    template <class T> T *alloct()
    {
        return (T *)this->alloc(sizeof(T));
    }
};
