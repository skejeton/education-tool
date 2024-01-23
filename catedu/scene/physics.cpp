#include "physics.hpp"

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

void PhysicsWorld::resolve_physics(PhysicsManifolds &manifolds)
{
    for (auto [_, manifold] : iter(manifolds.manifolds))
    {
        PhysicsBody &a = bodies.get_assert(manifold.first);
        PhysicsBody &b = bodies.get_assert(manifold.second);

        if (a.solid && b.solid)
        {
            resolve_bodies(a, b);
        }
    }
}

PhysicsManifolds PhysicsWorld::detect_collisions()
{
    PhysicsManifolds r = {};

    for (auto [id, body] : iter(bodies))
    {
        if (body.dynamic)
        {
            for (auto [id2, body2] : iter(bodies))
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
