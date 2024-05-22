#include "world.hpp"

#define BUILDING_DIMENSIONS_W 8
#define BUILDING_DIMENSIONS_D 8

World World::create()
{
    World world = {};

    world.space = Space::create();
    world.buildings = FreeList<Building>::create(Arena::create_malloc());

    return world;
}

Building *building_space(int x, int y, FreeList<Building> &buildings)
{
    for (auto &building : iter(buildings))
    {
        if (building.x - BUILDING_DIMENSIONS_W / 2 <= x &&
            building.x + BUILDING_DIMENSIONS_W / 2 >= x &&
            building.y - BUILDING_DIMENSIONS_D / 2 <= y &&
            building.y + BUILDING_DIMENSIONS_D / 2 >= y)
        {
            return &building;
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

    Building *building = buildings.alloc();

    building->floors = floors;
    building->x = x;
    building->y = y;
}

void World::remove_building(int x, int y)
{
    if (Building *building = building_space(x, y, buildings);
        building != nullptr)
    {
        x = building->x;
        y = building->y;
        space.unclaim_region_rect({x - 4, y - 4, 8, 8});
        buildings.free(building);
    }
}
