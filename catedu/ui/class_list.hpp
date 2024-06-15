#pragma once

#include "catedu/core/alloc/free_list.hpp"

struct UiClassList;
// The tree is sorted in an alphabetical order
struct UiClassNode
{
    UiClassNode *left, *right;
    UiClassList *sub;
};

struct UiClassList
{
    char *name;
    void *data;
    FreeList<UiClassNode> class_nodes;
    UiClassNode *root;

    static UiClassList create(Arena alloc);
    void destroy();
    UiClassList *add_class(const char *cl, void *data);
    UiClassList *get_class(const char *cl);
};
