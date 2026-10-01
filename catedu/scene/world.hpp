#pragma once
#include "catedu/core/alloc/free_list.hpp"
#include "catedu/scene/space.hpp"
#include "catedu/scene/inventory.hpp"
#include "script.hpp"

struct Place;

struct Object
{
    enum class Type : int
    {
        building,
        road,
        player,
        wall,
        tree,
        npc,
        item,
        animal,
        water,
        high_grass,
        prop,
        bridge,
        castle
    } type;

    int floors;
    float x, y, z;
    Place *place;
    char id[32];
};

// The object type's name in exported files.
const char *object_type_name(Object::Type type);
// Finds the object type with the name `object_type_name` gives it.
bool object_type_from_name(const char *name, Object::Type *type);

struct Place
{
    bool interior;
    Space space;
    FreeList<Object> objects;

    static Place create();
    void destroy();
    Place clone();

    Object *place_object(Object object);
    void remove_object(int x, int y);
    void remove_object(int x, int y, Object::Type type);
    Object *get_object_at(int x, int y);
    Object *get_object_at(int x, int y, Object::Type type);
    RectI object_bounds(Object &object);

    bool can_place_building(int floors, int x, int y);
    bool can_place_objtype(Object::Type type, int x, int y);
};

struct World
{
    FreeList<Place> places;
    Script *script;
    Place *first;
    Place *current;

    Inventory player_inventory;
    float time_of_day;

    static World create();
    void destroy();
    World clone();
};
