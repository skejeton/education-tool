#pragma once
#include "catedu/core/alloc/free_list.hpp"
#include "catedu/scene/space.hpp"

struct Object
{
    enum class Type
    {
        Building,
        Road,
        Player
    } type;

    int floors;
    float x, y;
};

struct World
{
    Space space;
    FreeList<Object> objects;

    static World create();
    void destroy();
    World clone();

    Object *place_object(Object object);
    void remove_object(int x, int y);
    Object *get_object_at(int x, int y);

    bool can_place_building(int floors, int x, int y);
    bool can_place_road(int x, int y);
};
