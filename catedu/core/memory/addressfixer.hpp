#pragma once

#include "catedu/core/storage/stack.hpp"

template <class T> struct AddressFixer
{
    Stack<std::pair<T *, T *>> mappings;
    Stack<T **> pointers;

    void add_pointer(T **pointer)
    {
        if (*pointer == nullptr)
            return;
        pointers.push(pointer);
    }

    void add_mapping(T *a, T *b)
    {
        mappings.push({a, b});
    }

    T *find_mapping(T *pointer)
    {
        for (int i = 0; i < mappings.count; i++)
        {
            if (pointer == mappings[i].first)
            {
                return mappings[i].second;
            }
        }

        assert(false && "No mapping found");
    }

    void fix_addresses()
    {
        for (int i = 0; i < pointers.count; i++)
        {
            *pointers[i] = find_mapping(*pointers[i]);
        }
    }

    static AddressFixer create()
    {
        return {};
    }

    void destroy()
    {
        mappings.deinit();
        pointers.deinit();
    }
};
