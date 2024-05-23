#include "world.hpp"

#define BUILDING_DIMENSIONS_W 8
#define BUILDING_DIMENSIONS_D 8

World World::create()
{
    World world = {};

    world.space = Space::create();
    world.objects = FreeList<Object>::create(Arena::create_malloc());

    return world;
}

RectI object_dimensions(Object &object)
{
    switch (object.type)
    {
    case Object::Type::Building:
        return {object.x - BUILDING_DIMENSIONS_W / 2,
                object.y - BUILDING_DIMENSIONS_D / 2, BUILDING_DIMENSIONS_W,
                BUILDING_DIMENSIONS_D};
    case Object::Type::Road:
        return {object.x - 1, object.y - 1, 2, 2};
    }

    assert(false);
}

bool point_in_dimensions(int x, int y, RectI dimensions)
{
    return dimensions.pos.x <= x && dimensions.pos.x + dimensions.siz.x >= x &&
           dimensions.pos.y <= y && dimensions.pos.y + dimensions.siz.y >= y;
}

Object *object_space(int x, int y, FreeList<Object> &objects)
{
    for (auto &object : iter(objects))
    {
        if (point_in_dimensions(x, y, object_dimensions(object)))
        {
            return &object;
        }
    }

    return nullptr;
}

void World::add_building(int floors, int x, int y)
{
    RectI region = {x - 4, y - 4, 8, 8};

    if (space.is_region_claimed(region))
    {
        return;
    }
    space.claim_region_rect(region);

    Object *building = objects.alloc();

    building->type = Object::Type::Building;
    building->floors = floors;
    building->x = x;
    building->y = y;
}

void World::add_road(int x, int y)
{
    RectI region = {x - 1, y - 1, 2, 2};

    if (space.is_region_claimed(region))
    {
        return;
    }
    space.claim_region_rect(region);

    Object *road = objects.alloc();

    road->type = Object::Type::Road;
    road->x = x;
    road->y = y;
}

bool World::can_place_building(int floors, int x, int y)
{
    RectI region = {x - 4, y - 4, 8, 8};

    if (space.is_region_claimed(region))
    {
        return false;
    }

    return true;
}

bool World::can_place_road(int x, int y)
{
    RectI region = {x - 1, y - 1, 2, 2};

    if (space.is_region_claimed(region))
    {
        return false;
    }

    return true;
}

void World::remove_object(int x, int y)
{
    if (Object *obj = object_space(x, y, objects); obj != nullptr)
    {
        space.unclaim_region_rect(object_dimensions(*obj));
        objects.free(obj);
    }
}
