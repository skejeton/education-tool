#pragma once
template <class T> T *zeroinit(T *v)
{
    *v = {};
    return v;
}
