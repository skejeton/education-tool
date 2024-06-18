#include "catedu/gui/editor/camera.hpp"
#include "catedu/gui/transition/transition.hpp"
#include "catedu/scene/physics.hpp"
#include "catedu/scene/world.hpp"
#include "catedu/ui/user.hpp"
#include "script.hpp"
#include <catedu/sys/input.hpp>

struct Playtest
{
    TableId player;
    PhysicsWorld physics;
    World world;
    Place *switch_target;
    Script *script;
    size_t script_current;

    static Playtest create(World world, Script &script);
    void destroy();

    void update(UiPass &user, Input &input, EditorCamera &camera,
                GuiTransition &transition);
};
