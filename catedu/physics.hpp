#pragma once
#include "catedu/rendering/3d/boxdraw.hpp"
#include "catedu/math.hpp"
#include "catedu/table.hpp"
#include "catedu/rendering/resource_spec.hpp"

struct PhysicsBody
{
    Rect area;
    bool solid;
    bool dynamic;
    int tag;
};

struct PhysicsManifold
{
    TableId first;
    TableId second;
};

struct PhysicsManifolds
{
    Table<PhysicsManifold> manifolds;
};

struct PhysicsWorld
{
    Table<PhysicsBody> bodies;
};

void resolve_dynamic_vs_static(PhysicsBody &dynamic, PhysicsBody static_);

PhysicsManifolds dGGetect_collisions(PhysicsWorld &world);
void resolve_physics(PhysicsWorld &world, PhysicsManifolds &manifolds);

struct WorldPrototype
{
    char tiles[16 * 16];
};

struct WorldState
{
    PhysicsWorld physics;
    TableId player_id;

    void deinit()
    {
        physics.bodies.deinit();
    }
};

WorldState world_prototype_to_world_state(WorldPrototype &prototype);
Rect &world_state_player_rect(WorldState &world);
void render_physics_world_via_boxdraw(PhysicsWorld &world,
                                      BoxdrawRenderer &boxdraw,
                                      ResourceSpec &resources);
bool move_player(WorldState &world, Vector2 delta, int &floor);
