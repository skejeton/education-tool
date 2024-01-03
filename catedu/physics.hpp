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

    PhysicsManifolds detect_collisions();
    void resolve_physics(PhysicsManifolds &manifolds);
};
