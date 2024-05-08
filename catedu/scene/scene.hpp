#pragma once

#include "catedu/core/memory/buffer.hpp"
#include "catedu/rendering/3d/pbr.hpp"
#include "obj_entity.hpp"
#include "obj_tilemap.hpp"
#include "physics.hpp"

struct Object
{
    char id[32];
    char name[32];
    bool hide;

    enum Type
    {
        Generic,
        Tilemap,
        Entity
    };

    Type type;
    union {
        ObjTilemap tilemap;
        ObjEntity entity;
    };

    Object copy();
};

typedef TableId ObjectId;

struct Scene
{
    Table<Object> objects;
    PhysicsWorld physics;

    static Scene init();
    ObjectId add_object(Object object);
    ObjectId find_object(const char *id);

    /// @brief Removes the object without freeing the memory
    Object prune_object(ObjectId id);
    Object *get_object(ObjectId id);

    void remove_object(ObjectId id);

    static Object object(ObjTilemap);
    static Object object(ObjEntity);

    Scene copy();

    Buffer save();
    static Scene load(Buffer buffer);

    void update(ResourceSpec &resources);
    void render(catedu::pbr::Renderer &renderer, ResourceSpec &resources,
                bool show_physics = false);
    void deinit();
};
