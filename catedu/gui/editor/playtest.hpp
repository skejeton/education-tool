#include "catedu/gui/editor/camera.hpp"
#include "catedu/gui/transition/transition.hpp"
#include "catedu/scene/physics.hpp"
#include "catedu/ui/user.hpp"
#include "world.hpp"
#include <catedu/sys/input.hpp>

struct Playtest
{
    TableId player;
    PhysicsWorld physics;
    World world;
    Place *switch_target;

    static Playtest create(World world);
    void destroy();

    void update(UiUser &user, Input &input, EditorCamera &camera,
                GuiTransition &transition);
};
