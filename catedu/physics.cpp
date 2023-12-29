#include "physics.hpp"

#define TAG_WALL 0
#define TAG_PLAYER 1
#define TAG_BOX 2
#define TAG_UP 3
#define TAG_DOWN 4

void resolve_bodies(PhysicsBody &a, PhysicsBody b)
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

void resolve_physics(PhysicsWorld &world, PhysicsManifolds &manifolds)
{
    for (auto [_, manifold] : iter(manifolds.manifolds))
    {
        PhysicsBody &a = world.bodies.get_assert(manifold.first);
        PhysicsBody &b = world.bodies.get_assert(manifold.second);

        if (a.solid && b.solid)
        {
            resolve_bodies(a, b);
        }
    }
}

int update_floor(int current, PhysicsWorld &world, PhysicsManifolds &manifolds)
{
    for (auto [_, manifold] : iter(manifolds.manifolds))
    {
        PhysicsBody &a = world.bodies.get_assert(manifold.first);
        PhysicsBody &b = world.bodies.get_assert(manifold.second);

        if (a.tag == TAG_UP && b.tag == TAG_PLAYER)
        {
            return current + 1;
        }
        if (a.tag == TAG_DOWN && b.tag == TAG_PLAYER)
        {
            return current - 1;
        }
    }
    return current;
}

PhysicsManifolds detect_collisions(PhysicsWorld &world)
{
    PhysicsManifolds r = {};

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

                if (rect_vs_rect(body.area, body2.area))
                {
                    r.manifolds.allocate({id, id2});
                }
            }
        }
    }

    return r;
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
                    {{Vector2{(float)x, (float)y}, Vector2{1, 1}},
                     true,
                     false,
                     TAG_WALL});
            }
            if (tile == 'P')
            {
                r.player_id = r.physics.bodies.allocate(
                    {{Vector2{(float)x, (float)y}, Vector2{0.8, 0.8}},
                     true,
                     true,
                     TAG_PLAYER});
            }
            if (tile == 'D')
            {
                r.physics.bodies.allocate(
                    {{Vector2{(float)x, (float)y}, Vector2{1, 1}},
                     true,
                     true,
                     TAG_BOX});
            }
            if (tile == '^')
            {
                r.physics.bodies.allocate(
                    {{Vector2{(float)x, (float)y}, Vector2{1, 1}},
                     false,
                     true,
                     TAG_UP});
            }
            if (tile == 'v')
            {
                r.physics.bodies.allocate(
                    {{Vector2{(float)x, (float)y}, Vector2{1, 1}},
                     false,
                     true,
                     TAG_DOWN});
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
        Box3 box = {{body.area.pos.x, 0, body.area.pos.y},
                    {body.area.pos.x + body.area.siz.x, 1,
                     body.area.pos.y + body.area.siz.y}};

        const char *model_name;
        switch (body.tag)
        {
        case TAG_WALL:
            model_name = "bricks";
            break;
        case TAG_PLAYER:
            model_name = "player";
            break;
        case TAG_BOX:
            model_name = "counter";
            break;
        case TAG_UP:
            model_name = "stairs_up";
            break;
        case TAG_DOWN:
            model_name = "stairs_down";
            break;
        }

        SpecModel *model =
            resources.models.get(resources.find_model_by_name(model_name));
        assert(model);

        BoxdrawCommand cmd = boxdraw_cmdtexture(
            box, resources.tileset.cropped(model->texture_rect));

        boxdraw_push(&boxdraw, cmd);
    }
}

bool move_player(WorldState &world, Vector2 delta, int &floor)
{
    int old_floor = floor;

    PhysicsBody &player = world.physics.bodies.get_assert(world.player_id);
    player.area.pos += delta;

    PhysicsManifolds manifolds = detect_collisions(world.physics);
    resolve_physics(world.physics, manifolds);
    floor = update_floor(floor, world.physics, manifolds);

    return floor != old_floor;
}
