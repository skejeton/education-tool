#include "space.hpp"

#define CLAIMED 1
#define UNCLAIMED 0

void Space::claim_region_rect(RectI region)
{
    for (int y = region.pos.y; y < region.pos.y + region.siz.y; y++)
    {
        for (int x = region.pos.x; x < region.pos.x + region.siz.x; x++)
        {
            claimed_tiles.set_tile({x, y}, CLAIMED);
        }
    }
}

void Space::unclaim_region_rect(RectI region)
{
    for (int y = region.pos.y; y < region.pos.y + region.siz.y; y++)
    {
        for (int x = region.pos.x; x < region.pos.x + region.siz.x; x++)
        {
            claimed_tiles.set_tile({x, y}, UNCLAIMED);
        }
    }
}

bool Space::is_region_claimed(RectI region)
{
    for (int y = region.pos.y; y < region.pos.y + region.siz.y; y++)
    {
        for (int x = region.pos.x; x < region.pos.x + region.siz.x; x++)
        {
            if (claimed_tiles.get_tile({x, y}) == CLAIMED)
            {
                return true;
            }
        }
    }
    return false;
}
