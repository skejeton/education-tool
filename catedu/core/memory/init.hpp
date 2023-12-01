#ifndef HPP_CATEDU_CORE_MEMORY_INIT
#define HPP_CATEDU_CORE_MEMORY_INIT

template <class T> T *zeroinit(T *v)
{
    *v = {};
    return v;
}

#endif // HPP_CATEDU_CORE_MEMORY_INIT
