#include "grid.hpp"
#include "construct.hpp"

#define GRID_COLOR 0xFFFF77FF

GeneratedObject genmesh_generate_grid(int size_x, int size_z)
{
    GeneratedObject object = {};

    // Generate horizontal grid lines
    for (int x = 1; x < size_x; x++)
    {
        const float center = size_x / 2.0f;
        const float distance = abs(x - center) / center;
        Color color = Color::hex(GRID_COLOR);
        color.a *= 1.0 - distance;

        Vector3 start = {(float)(x - size_x / 2.0f), 0.0, 0.0};
        Box3 box = construct_box(Baseline::Center, {0.1, 0.1, (float)size_z});
        box = box3_translate(box, {start.x, start.y, start.z});
        object.push_colored_box(box, color);
    }

    // Generate vertical grid lines
    for (int z = 1; z < size_z; z++)
    {
        const float center = size_z / 2.0f;
        const float distance = abs(z - center) / center;
        Color color = Color::hex(GRID_COLOR);
        color.a *= 1.0 - distance;

        Vector3 start = {0.0, 0.0, (float)(z - size_z / 2.0f)};
        Box3 box = construct_box(Baseline::Center, {(float)size_x, 0.1, 0.1});
        box = box3_translate(box, {start.x, start.y, start.z});
        object.push_colored_box(box, color);
    }

    return object;
}
