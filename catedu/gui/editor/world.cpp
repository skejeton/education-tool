#include "world.hpp"

#define BUILDING_DIMENSIONS_W 8
#define BUILDING_DIMENSIONS_D 8

World World::create()
{
    return World();
}

int building_space(int x, int y, Building *buildings, int num_buildings)
{
    for (int i = 0; i < num_buildings; i++)
    {
        if (buildings[i].x - BUILDING_DIMENSIONS_W / 2 <= x &&
            buildings[i].x + BUILDING_DIMENSIONS_W / 2 >= x &&
            buildings[i].y - BUILDING_DIMENSIONS_D / 2 <= y &&
            buildings[i].y + BUILDING_DIMENSIONS_D / 2 >= y)
        {
            return i;
        }
    }
    return -1;
}

bool area_occupied(World *world, int x, int y, int w, int h)
{
    for (int i = 0; i < world->num_buildings; i++)
    {
        if (world->buildings[i].x - BUILDING_DIMENSIONS_W / 2 <= x + w &&
            world->buildings[i].x + BUILDING_DIMENSIONS_W / 2 >= x &&
            world->buildings[i].y - BUILDING_DIMENSIONS_D / 2 <= y + h &&
            world->buildings[i].y + BUILDING_DIMENSIONS_D / 2 >= y)
        {
            return true;
        }
    }
    return false;
}

void World::add_building(int floors, int x, int y)
{
    if (area_occupied(this, x - 4, y - 4, 8, 8))
    {
        return;
    }

    buildings[num_buildings].floors = floors;
    buildings[num_buildings].x = x;
    buildings[num_buildings].y = y;
    num_buildings++;
}

void World::remove_building(int x, int y)
{
    if (int i = building_space(x, y, buildings, num_buildings); i != -1)
    {
        for (int j = i; j < num_buildings - 1; j++)
        {
            // move buildings to the left
            buildings[j] = buildings[j + 1];
        }
        num_buildings--;
    }
}
