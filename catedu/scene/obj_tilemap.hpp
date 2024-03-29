#pragma once
#include "basic_tilemap.hpp"
#include "catedu/core/alloc/bump_allocator.hpp"
#include "catedu/rendering/3d/pbr.hpp"
#include "catedu/rendering/resource_spec.hpp"
#include "physics.hpp"

struct ObjTilemap
{
    BasicTilemap tilemap;
    bool physics_init;

    static ObjTilemap init();
    void deinit();

    void set_tile(Vector2i tile_position, int tile_id);
    int get_tile(Vector2i tile_position);

    void update(PhysicsWorld &world, ResourceSpec &resources);
    void render(catedu::pbr::Renderer &renderer, ResourceSpec &resources);

    ObjTilemap copy();

    static ObjTilemap load(void **data);
    void save(BumpAllocator &alloc);
};
