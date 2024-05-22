#pragma once
#include "catedu/core/alloc/free_list.hpp"
#include "catedu/scene/space.hpp"

struct Building
{
    int floors;
    int x, y;
};

struct World
{
    Space space;
    FreeList<Building> buildings;

    static World create();
    void add_building(int floors, int x, int y);
    void remove_building(int x, int y);
};
