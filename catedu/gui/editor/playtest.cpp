#include "playtest.hpp"

Playtest Playtest::create(World world)
{
    return {world};
}

void Playtest::update(Input &input)
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

    for (auto &obj : iter(world.objects))
    {
        if (obj.type == Object::Type::Player)
        {
            obj.x += movement.x;
            obj.y += movement.y;
        }
    }
}
