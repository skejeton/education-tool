#include "playtest.hpp"
#include "catedu/gui/transition/transition.hpp"
#include "catedu/scene/physics.hpp"
#include "catedu/sys/input.hpp"
#include "catedu/ui/layout/autolayout.hpp"
#include "catedu/ui/rendering/make_brush.hpp"

PhysicsWorld create_bodies(Place *parent, Place &place, TableId &player)
{
    PhysicsWorld physics = {};
    for (auto &obj : iter(place.objects))
    {
        if (obj.type == Object::Type::Wall)
        {
            PhysicsBody body = {};
            body.area = {obj.x + 0.5f, obj.y + 0.5f, 1, 1};
            body.solid = true;
            body.dynamic = false;

            physics.bodies.allocate(body);
        }
        if (obj.type == Object::Type::Building)
        {
            PhysicsBody body = {};
            body.area = {obj.x - 4 + 0.5f, obj.y - 4 + 0.5f, 8, 8};
            body.solid = true;
            body.dynamic = false;

            physics.bodies.allocate(body);

            PhysicsBody door = {};
            door.area = {obj.x, obj.y - 4.1f, 1, 1};
            door.solid = false;
            door.dynamic = false;
            door.userdata = obj.place;

            physics.bodies.allocate(door);
        }
        if (obj.type == Object::Type::Player)
        {
            PhysicsBody body = {};
            body.area = {obj.x, obj.y, 1, 1};
            body.solid = true;
            body.dynamic = true;

            player = physics.bodies.allocate(body);
        }
    }
    if (place.interior)
    {
        PhysicsBody door = {};
        door.area = {0, -7, 2, 1};
        door.solid = false;
        door.dynamic = false;
        door.userdata = parent;

        physics.bodies.allocate(door);
    }

    return physics;
}

Playtest Playtest::create(World world)
{
    TableId player = {};
    PhysicsWorld physics = create_bodies(nullptr, *world.first, player);
    return {player, physics, world};
}

void Playtest::destroy()
{
    physics.bodies.deinit();
    world.destroy();
}

void Playtest::update(UiUser &user, Input &input, EditorCamera &camera,
                      GuiTransition &transition)
{
    bool enter = false;
    Vector2 movement = {0, 0};

    if (input.k[SAPP_KEYCODE_UP].held)
        movement.y += 1;
    if (input.k[SAPP_KEYCODE_DOWN].held)
        movement.y -= 1;
    if (input.k[SAPP_KEYCODE_LEFT].held)
        movement.x -= 1;
    if (input.k[SAPP_KEYCODE_RIGHT].held)
        movement.x += 1;
    if (input.k[SAPP_KEYCODE_SPACE].pressed)
        enter = true;

    movement *= 0.2f;

    if (world.current->interior)
    {
        movement *= -1.0f;
    }

    PhysicsBody &player = physics.bodies.get_assert(this->player);
    player.area.pos.x += movement.x;
    player.area.pos.y += movement.y;

    PhysicsManifolds manifolds = physics.detect_collisions();
    physics.resolve_physics(manifolds);
    for (auto [id, manifold] : iter(manifolds.manifolds))
    {
        PhysicsBody &b = physics.bodies.get_assert(manifold.second);

        if (manifold.first == this->player && b.userdata)
        {
            if (enter)
            {
                void *userdata = b.userdata;
                switch_target = (Place *)userdata;
                transition.begin();
                break;
            }
        }
    }
    manifolds.manifolds.deinit();

    player = physics.bodies.get_assert(this->player);

    for (auto &obj : iter(world.current->objects))
    {
        if (obj.type == Object::Type::Player)
        {
            obj.x = player.area.pos.x;
            obj.y = player.area.pos.y;

            if (world.current->interior)
            {
                camera.follow({obj.x, 0, obj.y}, 3.1415);
            }
            else
            {
                camera.follow({obj.x, 0, obj.y}, 0);
            }
        }
    }

    if (transition.switching())
    {
        physics.bodies.deinit();
        world.current = switch_target;
        physics = create_bodies(world.first, *world.current, this->player);

        PhysicsBody &player = physics.bodies.get_assert(this->player);

        camera.lockin({player.area.pos.x, 0, player.area.pos.y},
                      world.current->interior ? 3.1415 : 0);
        switch_target = nullptr;
    }
}
