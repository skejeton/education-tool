#include "grid.hpp"
#include "construct.hpp"

#define GRID_COLOR 0xFFFF77FF

GeneratedObject genmesh_generate_grid(int size_x, int size_z)
{
    GeneratedObject object = {};

    // Generate horizontal grid lines
    for (int x = 1; x < size_x; x++)
    {
        Vector3 start = {(float)(x - size_x / 2.0f), 0.0, 0.0};
        Box3 box = construct_box(Baseline::Center, {0.1, 0.1, (float)size_z});
        box = box3_translate(box, {start.x, start.y, start.z});
        object.push_colored_box(box, Color::hex(GRID_COLOR));
    }

    // Generate vertical grid lines
    for (int z = 1; z < size_z; z++)
    {
        Vector3 start = {0.0, 0.0, (float)(z - size_z / 2.0f)};
        Box3 box = construct_box(Baseline::Center, {(float)size_x, 0.1, 0.1});
        box = box3_translate(box, {start.x, start.y, start.z});
        object.push_colored_box(box, Color::hex(GRID_COLOR));
    }

    return object;
}
