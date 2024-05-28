#include "catedu/gui/editor/camera.hpp"
#include "catedu/scene/physics.hpp"
#include "world.hpp"
#include <catedu/sys/input.hpp>

struct Playtest
{
    TableId player;
    PhysicsWorld physics;
    World world;

    static Playtest create(World world);
    void destroy();

    void update(Input &input, EditorCamera &camera);
};
