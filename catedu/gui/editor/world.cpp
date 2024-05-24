#include "world.hpp"

#define BUILDING_DIMENSIONS_W 8
#define BUILDING_DIMENSIONS_D 8

World World::create()
{
    World world = {};

    world.space = Space::create();
    world.objects = FreeList<Object>::create(Arena::create(&ALLOCATOR_MALLOC));

    return world;
}

void World::destroy()
{
    objects.destroy();
    space.destroy();
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
        return {object.x - 2, object.y - 2, 4, 4};
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

Object *World::place_object(Object object)
{
    RectI region = object_dimensions(object);

    if (space.is_region_claimed(region))
    {
        return nullptr;
    }

    space.claim_region_rect(region);

    Object *obj = objects.alloc();
    *obj = object;

    return obj;
}

bool World::can_place_building(int floors, int x, int y)
{
    RectI region = {x - 4, y - 4, 8, 8};

    return !space.is_region_claimed(region);
}

bool World::can_place_road(int x, int y)
{
    RectI region = {x - 2, y - 2, 4, 4};

    return !space.is_region_claimed(region);
}

void World::remove_object(int x, int y)
{
    if (Object *obj = object_space(x, y, objects); obj != nullptr)
    {
        space.unclaim_region_rect(object_dimensions(*obj));
        objects.free(obj);
    }
}

Object *World::get_object_at(int x, int y)
{
    return object_space(x, y, objects);
}
