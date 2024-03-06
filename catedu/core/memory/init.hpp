#pragma once
template <class T> T *zeroinit(T *v)
{
    memset(v, 0, sizeof(T));
    return v;
}
