#include "class_list.hpp"
#include "catedu/core/alloc/free_list.hpp"

static UiClassNode *alloc_node(FreeList<UiClassNode> &list, const char *name)
{
    char *namecpy = (char *)list.arena.alloc(strlen(name) + 1);
    memcpy(namecpy, name, strlen(name) + 1);

    UiClassNode *node = list.alloc({});
    node->name = namecpy;

    return node;
}

UiClassList UiClassList::create(Arena alloc)
{
    UiClassList result = {};
    result.class_nodes = FreeList<UiClassNode>::create(alloc);
    return result;
}

void UiClassList::destroy()
{
    this->class_nodes.destroy();
}

void UiClassList::add_class(const char *cl, void *data)
{
    if (this->root == nullptr)
    {
        this->root = alloc_node(class_nodes, cl);
        return;
    }

    UiClassNode *node = this->root;
    while (true)
    {
        switch (strcmp(cl, node->name))
        {
        case 0:
            // class exists...
            return;
        case -1:
            if (node->left)
            {
                node = node->left;
            }
            else
            {
                node->left = alloc_node(class_nodes, cl);
                return;
            }
            break;
        case 1:
            if (node->right)
            {
                node = node->right;
            }
            else
            {
                node->right = alloc_node(class_nodes, cl);
                return;
            }
            break;
        }
    }
}
