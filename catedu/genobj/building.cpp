#include "building.hpp"
#include "construct.hpp"
#include <cassert>

#define FLOOR_HEIGHT 2
#define BUILDING_SIZE_X 8
#define BUILDING_SIZE_Z 8
#define FLOOR_DIMENSIONS                                                       \
    Vector3                                                                    \
    {                                                                          \
        BUILDING_SIZE_X, FLOOR_HEIGHT, BUILDING_SIZE_Z                         \
    }
#define BUILDING_COLOR_BOTTOM 0x777777FF
#define BUILDING_COLOR_MIDDLE 0x999999FF
#define BUILDING_COLOR_WINDOW 0x000077FF
#define BUILDING_COLOR_DOOR 0x770000FF

GeneratedObject genmesh_generate_building(int floor_count)
{
    GeneratedObject object = {};

    assert(floor_count > 0);

    if (floor_count--)
    {
        object.push_colored_box(
            construct_box(Baseline::Bottom, FLOOR_DIMENSIONS),
            Color::hex(BUILDING_COLOR_BOTTOM));
    }
    // add windows
    for (int i = 0; i < floor_count; i++)
    {
        for (int x = 0; x < BUILDING_SIZE_X / 2; x++)
        {
            Vector3 pos = {(float)((x * 2 + 0.5) - BUILDING_SIZE_X / 2.0 + 0.5),
                           (float)(FLOOR_HEIGHT * (i + 1)) + 0.5f, 0.0};
            object.push_colored_box(
                box3_translate(
                    construct_box(Baseline::Bottom,
                                  {0.7, 0.9, BUILDING_SIZE_Z + 0.02}),
                    pos),
                Color::hex(BUILDING_COLOR_WINDOW));
        }

        for (int z = 0; z < BUILDING_SIZE_Z / 2; z++)
        {
            Vector3 pos = {
                0.0, (float)(FLOOR_HEIGHT * (i + 1)) + 0.5f,
                (float)((z * 2 + 0.5) - BUILDING_SIZE_Z / 2.0 + 0.5)};
            object.push_colored_box(
                box3_translate(
                    construct_box(Baseline::Bottom,
                                  {BUILDING_SIZE_X + 0.02, 0.9, 0.7}),
                    pos),
                Color::hex(BUILDING_COLOR_WINDOW));
        }
    }

    if (floor_count)
    {
        Vector3 size = FLOOR_DIMENSIONS * Vector3{1, (float)floor_count, 1};
        Vector3 pos = {0, FLOOR_HEIGHT, 0};

        object.push_colored_box(
            box3_translate(construct_box(Baseline::Bottom, size), pos),
            Color::hex(BUILDING_COLOR_MIDDLE));
    }

    object.push_colored_box(
        box3_translate(construct_box(Baseline::Bottom, {1, 2, 0.1}),
                       {0, 0, -4}),
        Color::hex(BUILDING_COLOR_DOOR));

    return object;
}
