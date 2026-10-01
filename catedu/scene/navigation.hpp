#pragma once
#include "world.hpp"
#include <vector>

// Whether a character can stand on the tile. Buildings, walls and trees block
// it; everything else, including roads and props, can be walked over.
bool is_walkable(Place *place, Vector2i pos);

// Finds a path between two tiles with A*, moving in four directions. The path
// excludes `start` and includes `end`. It's empty when `end` can't be reached
// within `max_iterations` expanded tiles, or when `start` is `end`.
std::vector<Vector2i> find_path(Place *place, Vector2i start, Vector2i end,
                                int max_iterations = 500);
