#ifndef HPP_CATEDU_PHYSICS
#define HPP_CATEDU_PHYSICS

#include "boxdraw.hpp"
#include "catedu/math.hpp"
#include "catedu/table.hpp"

struct PhysicsBody
{
    Rect area;
    bool dynamic;
};

struct PhysicsWorld
{
    Table<PhysicsBody> bodies;
};

void resolve_dynamic_vs_static(PhysicsBody &dynamic, PhysicsBody static_);
void resolve_physics(PhysicsWorld &world);

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
                                      BoxdrawRenderer &boxdraw);
void move_player(WorldState &world, Vector2 delta);

#endif
