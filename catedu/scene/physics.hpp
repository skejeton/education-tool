#pragma once

#include "catedu/core/math/math.hpp"
#include "catedu/core/storage/table.hpp"
#include "catedu/rendering/resource_spec.hpp"

struct PhysicsBody
{
    Rect area;
    bool solid;
    bool dynamic;
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
