#pragma once
#include "catedu/core/alloc/free_list.hpp"
#include "catedu/scene/space.hpp"

struct Object
{
    enum class Type
    {
        Building,
        Road,
    } type;

    int floors;
    int x, y;
};

struct World
{
    Space space;
    FreeList<Object> objects;

    static World create();
    void destroy();
    void add_building(int floors, int x, int y);
    void add_road(int x, int y);
    bool can_place_building(int floors, int x, int y);
    bool can_place_road(int x, int y);
    void remove_object(int x, int y);
};
