#pragma once

#include "catedu/core/alloc/free_list.hpp"

// The tree is sorted in an alphabetical order
struct UiClassNode
{
    char *name;
    UiClassNode *left, *right;

    void *data;
};

struct UiClassList
{
    FreeList<UiClassNode> class_nodes;
    UiClassNode *root;

    static UiClassList create(Arena alloc);
    void destroy();
    void add_class(const char *cl, void *data);
};
