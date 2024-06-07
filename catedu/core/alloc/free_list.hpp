///
/// Free list allocator for fast entity allocation at a fixed place.
///

#pragma once
#include "arena.hpp"
#include <assert.h>
#include <iterator>

#define _FREELIST_DUMMYPTR (Node *)(size_t)0xDEADBEEF

#ifdef _FREELIST_SANITYCHECKS
int rand();
#endif

template <class T> struct FreeList
{
    struct alignas(Arena::ALIGN) Node
    {
        T data;
#ifdef _FREELIST_SANITYCHECKS
        uint64_t check;
#endif
        Node *prev, *next;
    };

    Arena arena;
    Node *freed;
    Node *first;
    Node *last;
#ifdef _FREELIST_SANITYCHECKS
    uint64_t check;
#endif

    static FreeList create(Arena arena);
    void destroy();

    T *alloc();
    T *alloc(T);
    void free(T *);
};

template <class T> inline FreeList<T> FreeList<T>::create(Arena arena)
{
    FreeList fl = {};
    fl.arena = arena;
#ifdef _FREELIST_SANITYCHECKS
    fl.check = rand();
#endif
    return fl;
}

template <class T> inline void FreeList<T>::destroy()
{
    this->arena.destroy();
}

template <class T> inline T *FreeList<T>::alloc()
{
    auto link_node = [&](Node *node) {
        node->next = nullptr;

        node->prev = last;
        if (last != nullptr)
        {
            last->next = node;
        }

        last = node;
        if (first == nullptr)
        {
            first = node;
        }
    };

    if (this->freed)
    {
        Node *node = this->freed;
        this->freed = node->next;

        link_node(node);

        return &node->data;
    }

    Node *node = (Node *)this->arena.alloc(sizeof(Node));

    link_node(node);

#ifdef _FREELIST_SANITYCHECKS
    node->check = this->check;
#endif

    return &node->data;
}

template <class T> inline T *FreeList<T>::alloc(T v)
{
    T *ptr = alloc();
    *ptr = v;
    return ptr;
}

template <class T> inline void FreeList<T>::free(T *ptr)
{
    Node *node = (Node *)ptr;
#ifdef _FREELIST_SANITYCHECKS
    assert(node->check == this->check);
#endif
    assert(node->prev != _FREELIST_DUMMYPTR && "Freeing already freed memory");

    if (node == last)
    {
        last = node->prev;
    }
    if (node == first)
    {
        first = node->next;
    }

    if (node->prev)
    {
        node->prev->next = node->next;
    }
    if (node->next)
    {
        node->next->prev = node->prev;
    }

    node->next = this->freed;
    node->prev = _FREELIST_DUMMYPTR;

    this->freed = node;
}

//------------------------------------------------------------------------------

template <class T> struct FreeListIterator
{
    using N = typename FreeList<T>::Node;
    N *node;

    static FreeListIterator create(FreeList<T> *fl);
    void skip();
    T *get();
    T *next();
};

template <class T>
inline FreeListIterator<T> FreeListIterator<T>::create(FreeList<T> *fl)
{
    FreeListIterator it = {};
    it.node = fl->first;
    return it;
}

template <class T> inline void FreeListIterator<T>::skip()
{
    assert(this->node);

    this->node = this->node->next;
}

template <class T> inline T *FreeListIterator<T>::get()
{
    return &this->node->data;
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
        return FreeListIterator<T>{};
    }

    bool operator==(const CxxFreeListIterator<T> &other) const
    {
        return iterator.node == other.iterator.node;
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
