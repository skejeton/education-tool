#pragma once
#include "catedu/core/alloc/free_list.hpp"
#include "catedu/scene/space.hpp"

struct Place;

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
    Place *inside;
};

struct Place
{
    Space space;
    FreeList<Object> objects;

    static Place create();
    void destroy();
    Place clone();

    Object *place_object(Object object);
    void remove_object(int x, int y);
    Object *get_object_at(int x, int y);

    bool can_place_building(int floors, int x, int y);
    bool can_place_objtype(Object::Type type, int x, int y);
};

struct World
{
    FreeList<Place> places;

    static World create();
    void destroy();
    World clone();
};
