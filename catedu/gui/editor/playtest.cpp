#include "playtest.hpp"

Playtest Playtest::create(World world)
{
    TableId player = {};
    PhysicsWorld physics = {};
    for (auto &obj : iter(world.objects))
    {
        if (obj.type == Object::Type::Building)
        {
            PhysicsBody body = {};
            body.area = {obj.x - 4 + 0.5f, obj.y - 4 + 0.5f, 8, 8};
            body.solid = true;
            body.dynamic = false;

            physics.bodies.allocate(body);
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

    return {player, physics, world};
}

void Playtest::destroy()
{
    physics.bodies.deinit();
    world.destroy();
}

void Playtest::update(Input &input, EditorCamera &camera)
{
    Vector2 movement = {0, 0};

    if (input.k[SAPP_KEYCODE_UP].held)
        movement.y += 1;
    if (input.k[SAPP_KEYCODE_DOWN].held)
        movement.y -= 1;
    if (input.k[SAPP_KEYCODE_LEFT].held)
        movement.x -= 1;
    if (input.k[SAPP_KEYCODE_RIGHT].held)
        movement.x += 1;

    movement *= 0.2f;

    PhysicsBody &player = physics.bodies.get_assert(this->player);

    player.area.pos.x += movement.x;
    player.area.pos.y += movement.y;

    PhysicsManifolds manifolds = physics.detect_collisions();
    physics.resolve_physics(manifolds);
    manifolds.manifolds.deinit();

    for (auto &obj : iter(world.objects))
    {
        if (obj.type == Object::Type::Player)
        {
            obj.x = player.area.pos.x;
            obj.y = player.area.pos.y;

            camera.follow({obj.x, 0, obj.y}, 0);
        }
    }
}
