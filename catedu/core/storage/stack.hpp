#pragma once

#include <assert.h>
#include <iterator>
#include <stdlib.h>
#include <string.h>

template <class T> struct Stack
{
    T *values;
    size_t capacity;
    size_t count;

    void deinit()
    {
        free(values);
    }

    bool scale()
    {
        size_t new_capacity = capacity;
        if (new_capacity == 0)
            new_capacity = 32;
        else
            new_capacity *= 2;

        T *new_values = (T *)realloc(values, sizeof(T) * new_capacity);
        // NOTE: Allocation error
        if (new_values == NULL)
        {
            return false;
        }

        values = new_values;
        capacity = new_capacity;

        return true;
    }

    void push(T value)
    {
        if (count == capacity)
        {
            if (!scale())
            {
                return;
            }
        }

        values[count++] = value;
    }

    T pop()
    {
        assert(count > 0);

        return values[--count];
    }

    T &top()
    {
        assert(count > 0);

        return values[count - 1];
    }

    T &operator[](size_t index)
    {
        assert(index < count);

        return values[index];
    }

    size_t size()
    {
        return count;
    }

    bool empty()
    {
        return count == 0;
    }
};

template <class T> struct StackIterator
{
    T *ptr;

    StackIterator(T *ptr) : ptr(ptr)
    {
    }

    T &operator*()
    {
        return *ptr;
    }

    StackIterator &operator++()
    {
        ptr++;
        return *this;
    }

    bool operator!=(const StackIterator &other)
    {
        return ptr != other.ptr;
    }
};
