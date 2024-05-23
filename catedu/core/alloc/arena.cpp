/// TODO: Handle *random* abnormally large allocations

#include "arena.hpp"
#include <cstdlib>

using Chunk = Arena::Chunk;

static size_t align_up(size_t size, size_t align)
{
    return (size + align - 1) & ~(align - 1);
}

static Chunk *create_chunk(Allocator *allocator, size_t cap)
{
    Chunk *chunk = (Chunk *)allocator->alloc(sizeof(Chunk) + cap);
    chunk->next = nullptr;
    chunk->size = 0;
    chunk->cap = cap;
    return chunk;
}

//------------------------------------------------------------------------------

Arena Arena::create(Allocator *allocator)
{
    Arena arena = {};
    arena.next_size = 1 << 12;
    arena.last = arena.first = create_chunk(allocator, arena.next_size);
    arena.allocator = allocator;
    return arena;
}

void Arena::destroy()
{
    Chunk *chunk = this->first;
    while (chunk)
    {
        Chunk *next = chunk->next;
        allocator->free(chunk);
        chunk = next;
    }
}

void Arena::reset()
{
    Chunk *chunk = this->first;
    while (chunk)
    {
        chunk->size = 0;
        chunk = chunk->next;
    }
}

void *Arena::alloc(size_t size)
{
    size = align_up(size, ALIGN);

    // 1. Check the current chunk
    Chunk *last = this->last;
    size_t remaining = last->cap - last->size;

    [[likely]] if (remaining >= size)
    {
        void *ptr = last->data + last->size;
        last->size += size;
        return ptr;
    }

    // 2. Find an existing chunk
    Chunk *chunk = this->first;
    while (chunk)
    {
        remaining = chunk->cap - chunk->size;
        if (remaining >= size)
        {
            void *ptr = chunk->data + chunk->size;
            chunk->size += size;
            return ptr;
        }
        chunk = chunk->next;
    }

    // 3. Create a new chunk
    this->next_size *= 2;
    size_t cap = this->next_size;
    if (size > cap)
    {
        cap = size;
    }

    Chunk *new_chunk = create_chunk(allocator, cap);
    new_chunk->size = size;
    this->last->next = new_chunk;
    this->last = new_chunk;
    return new_chunk->data;
}
