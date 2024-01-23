#pragma once

#include "obj_backdrop.hpp"
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
        Entity,
        Backdrop
    };

    Type type;
    union {
        ObjTilemap tilemap;
        ObjEntity entity;
        ObjBackdrop backdrop;
    };
};

typedef TableId ObjectId;

struct Scene
{
    Table<Object> objects;
    PhysicsWorld physics;

    static Scene init();
    ObjectId add_object(Object object);
    ObjectId find_object(const char *id);
    Object *get_object(ObjectId id);

    void remove_object(ObjectId id);

    static Object object(ObjTilemap);
    static Object object(ObjEntity);
    static Object object(ObjBackdrop);

    void update(ResourceSpec &resources);
    void render(BoxdrawRenderer &renderer, ResourceSpec &resources,
                bool show_physics = false);
    void deinit();
};
