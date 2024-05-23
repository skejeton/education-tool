///
/// Free list allocator for fast entity allocation at a fixed place.
///
/// WARNING: If you pass an arena that is in use, the free list will iterate
///          over unrelated data, causing undefined behavior.
////
/// TODO:    Either reserve an arena for itself, or use a linked list.
///

#pragma once
#include "arena.hpp"
#include <assert.h>
#include <iterator>

int rand();

template <class T> struct FreeList
{
    struct alignas(Arena::ALIGN) Node
    {
        T data;
        uint64_t free;
        uint64_t check;
        Node *next;
    };

    Arena arena;
    Node *freed;
    uint64_t check;

    static FreeList create(Arena arena);
    void destroy();

    T *alloc();
    void free(T *);
};

template <class T> inline FreeList<T> FreeList<T>::create(Arena arena)
{
    FreeList fl = {};
    fl.arena = arena;
    fl.check = rand();
    return fl;
}

template <class T> inline void FreeList<T>::destroy()
{
    this->arena.destroy();
}

template <class T> inline T *FreeList<T>::alloc()
{
    if (this->freed)
    {
        Node *node = this->freed;
        this->freed = node->next;
        node->free = 0;
        return &node->data;
    }

    Node *node = (Node *)this->arena.alloc(sizeof(Node));
    node->free = 0;
    node->check = this->check;
    node->next = nullptr;

    return &node->data;
}

template <class T> inline void FreeList<T>::free(T *ptr)
{
    Node *node = (Node *)ptr;
    assert(!node->free);
    node->next = this->freed;
    node->free = 1;
    this->freed = node;
}

//------------------------------------------------------------------------------

template <class T> struct FreeListIterator
{
    using N = typename FreeList<T>::Node;
    static const constexpr size_t S = sizeof(N);
    size_t index;
    Arena::Chunk *node;

    static FreeListIterator create(FreeList<T> *fl);
    void skip();
    T *get();
    T *next();
};

template <class T>
inline FreeListIterator<T> FreeListIterator<T>::create(FreeList<T> *fl)
{
    FreeListIterator it = {};
    it.node = fl->arena.first;
    it.skip();
    return it;
}

template <class T> inline void FreeListIterator<T>::skip()
{
    assert(this->node);

    while (this->node && this->index >= (this->node->size / S))
    {
        this->node = this->node->next;
        this->index = 0;
    }

    if (this->node && ((((N *)this->node->data)[this->index]).free))
    {
        index++;
        skip();
    }
}

template <class T> inline T *FreeListIterator<T>::get()
{
    return &((((N *)this->node->data)[this->index]).data);
}

template <class T> inline T *FreeListIterator<T>::next()
{
    assert(this->node);

    T *data = get();
    index++;
    skip();

    return data;
}

//------------------------------------------------------------------------------

template <class T> struct CxxFreeListIterator
{
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using reference = T &;

    FreeListIterator<T> iterator;

    CxxFreeListIterator(FreeListIterator<T> iterator) : iterator(iterator)
    {
    }

    CxxFreeListIterator begin()
    {
        return *this;
    }

    CxxFreeListIterator end()
    {
        FreeListIterator<T> it = iterator;
        it.node = nullptr;
        it.index = 0;
        return it;
    }

    bool operator==(const CxxFreeListIterator<T> &other) const
    {
        return iterator.node == other.iterator.node &&
               iterator.index == other.iterator.index;
    }

    bool operator!=(const CxxFreeListIterator<T> &other) const
    {
        return !(*this == other);
    }

    T &operator*()
    {
        return *iterator.get();
    }

    T *operator->()
    {
        return iterator.get();
    }

    CxxFreeListIterator &operator++()
    {
        iterator.next();
        return *this;
    }

    CxxFreeListIterator operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }
};

//------------------------------------------------------------------------------

template <class T> CxxFreeListIterator<T> iter(FreeList<T> &list)
{
    return CxxFreeListIterator<T>(FreeListIterator<T>::create(&list));
}
