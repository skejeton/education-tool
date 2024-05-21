/// TODO: Handle *random* abnormally large allocations

#include "arena.hpp"
#include <cstdlib>

using Chunk = Arena::Chunk;

static size_t align_up(size_t size, size_t align)
{
    return (size + align - 1) & ~(align - 1);
}

static Chunk *create_chunk(size_t size)
{
    Chunk *chunk = (Chunk *)malloc(sizeof(Chunk) + size);
    chunk->next = nullptr;
    chunk->size = 0;
    chunk->cap = size;
    return chunk;
}

//------------------------------------------------------------------------------

Arena Arena::create_malloc()
{
    Arena arena = {};
    arena.next_size = 1 << 16;
    arena.last = arena.first = create_chunk(arena.next_size);
    return arena;
}

void Arena::destroy()
{
    Chunk *chunk = this->first;
    while (chunk)
    {
        Chunk *next = chunk->next;
        free(chunk);
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
    this->last = this->first;
}

void *Arena::alloc(size_t size)
{
    size = align_up(size, 16);

    // 1. Check the current chunk
    Chunk *last = this->last;
    size_t remaining = last->cap - last->size;
    if (remaining >= size)
    {
        void *ptr = last->data + last->size;
        last->size += size;
        return ptr;
    }

    // 2. Find an existing chunk
    Chunk *chunk = this->first;
    while (chunk->next)
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

    Chunk *new_chunk = create_chunk(this->next_size);
    this->last->next = new_chunk;
    this->last = new_chunk;
    return new_chunk->data;
}
