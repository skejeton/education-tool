///
/// Free list allocator for fast entity allocation at a fixed place.
///

#pragma once
#include "arena.hpp"

template <class T> struct FreeList
{
    struct Node
    {
        T data;
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
        return &node->data;
    }

    return (T *)this->arena.alloc(sizeof(Node));
}

template <class T> inline void FreeList<T>::free(T *ptr)
{
    Node *node = (Node *)ptr;
    node->next = this->freed;
    this->freed = node;
}
