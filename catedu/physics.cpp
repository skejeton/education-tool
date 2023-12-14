#include "physics.hpp"

void resolve_bodies(PhysicsBody &a, PhysicsBody b)
{
    if (rect_vs_rect(a.area, b.area))
    {
        if (a.dynamic && !b.dynamic)
        {
            a.area.pos += rect_vs_rect_snap(a.area, b.area);
        }
        else if (!a.dynamic && b.dynamic)
        {
            b.area.pos += rect_vs_rect_snap(a.area, b.area);
        }
        else if (a.dynamic && b.dynamic)
        {
            Vector2 snap = rect_vs_rect_snap(a.area, b.area);
            a.area.pos += snap * 0.5f;
            b.area.pos -= snap * 0.5f;
        }
    }
}

void resolve_physics(PhysicsWorld &world)
{
    for (auto [id, body] : iter(world.bodies))
    {
        if (body.dynamic)
        {
            for (auto [id2, body2] : iter(world.bodies))
            {
                if (id == id2)
                {
                    continue;
                }

                resolve_bodies(body, body2);
            }
        }
    }
}

WorldState world_prototype_to_world_state(WorldPrototype &prototype)
{
    WorldState r = {};

    for (int y = 0; y < 16; y++)
    {
        for (int x = 0; x < 16; x++)
        {
            char tile = prototype.tiles[y * 16 + x];
            if (tile == 'X')
            {
                r.physics.bodies.allocate(
                    {{Vector2{(float)x, (float)y}, Vector2{1, 1}}, false});
            }
            if (tile == 'P')
            {
                r.player_id = r.physics.bodies.allocate(
                    {{Vector2{(float)x, (float)y}, Vector2{1, 1}}, true});
            }
            if (tile == 'D')
            {
                r.physics.bodies.allocate(
                    {{Vector2{(float)x, (float)y}, Vector2{1, 1}}, true});
            }
        }
    }

    return r;
}

Rect &world_state_player_rect(WorldState &world)
{
    return world.physics.bodies.get_assert(world.player_id).area;
}

void render_physics_world_via_boxdraw(PhysicsWorld &world,
                                      BoxdrawRenderer &boxdraw,
                                      ResourceSpec &resources)
{
    for (auto [id, body] : iter(world.bodies))
    {
        Box3 box = box3_extrude_from_point(
            {body.area.pos.x, 0, body.area.pos.y},
            Vector3{body.area.siz.x, 1, body.area.siz.y} * 0.5f);

        const char *model_name = body.dynamic ? "entity" : "counter";
        SpecModel *model =
            resources.models.get(resources.find_model_by_name(model_name));
        assert(model);

        BoxdrawCommand cmd = boxdraw_cmdtexture(
            box, resources.tileset.cropped(model->texture_rect));

        boxdraw_push(&boxdraw, cmd);
    }
}

void move_player(WorldState &world, Vector2 delta)
{
    PhysicsBody &player = world.physics.bodies.get_assert(world.player_id);
    player.area.pos += delta;
    resolve_physics(world.physics);
}
