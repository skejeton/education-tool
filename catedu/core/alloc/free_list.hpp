///
/// Free list allocator for fast entity allocation at a fixed place.
///

#pragma once
#include "arena.hpp"
#include <assert.h>
#include <iterator>

template <class T> struct FreeList
{
    struct Node
    {
        T data;
        uint64_t free;
        Node *next;
    };

    Arena arena;
    Node *freed;

    static FreeList create(Arena arena);
    void destroy();

    T *alloc();
    void free(T *);
};

template <class T> inline FreeList<T> FreeList<T>::create(Arena arena)
{
    FreeList fl = {};
    fl.arena = arena;
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
    node->next = nullptr;

    return &node->data;
}

template <class T> inline void FreeList<T>::free(T *ptr)
{
    Node *node = (Node *)ptr;
    node->next = this->freed;
    node->free = 1;
    this->freed = node;
}

//------------------------------------------------------------------------------

template <class T> struct FreeListIterator
{
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

    while (it.node &&
           it.index >= (it.node->size / sizeof(typename FreeList<T>::Node)))
    {
        it.node = it.node->next;
        it.index = 0;
    }

    return it;
}

template <class T> inline void FreeListIterator<T>::skip()
{
    assert(this->node);

    index++;

    while (this->node && this->index >= (this->node->size /
                                         sizeof(typename FreeList<T>::Node)))
    {
        this->node = this->node->next;
        this->index = 0;
    }

    if (this->node &&
        ((typename FreeList<T>::Node *)this->node->data + this->index)->free)
    {
        skip();
    }
}

template <class T> inline T *FreeListIterator<T>::get()
{
    return &((typename FreeList<T>::Node *)this->node->data + this->index)
                ->data;
}

template <class T> inline T *FreeListIterator<T>::next()
{
    assert(this->node);

    T *data = get();
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
