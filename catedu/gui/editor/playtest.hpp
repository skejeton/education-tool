#include "catedu/gui/editor/camera.hpp"
#include "world.hpp"
#include <catedu/sys/input.hpp>

struct Playtest
{
    World world;

    static Playtest create(World world);
    void update(Input &input, EditorCamera &camera);
};
