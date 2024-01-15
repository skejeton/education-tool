#include "scene.hpp"

Scene Scene::init()
{
    return Scene();
}

ObjectId Scene::add_object(Object object)
{
    return objects.allocate(object);
}

ObjectId Scene::find_object(const char *id)
{
    for (auto [id_, object] : iter(objects))
    {
        if (strcmp(id, object.id) == 0)
        {
            return id_;
        }
    }

    return {};
}

Object *Scene::get_object(ObjectId id)
{
    return objects.get(id);
}

void Scene::remove_object(ObjectId id)
{
    objects.remove(id);
}

Object Scene::object(ObjTilemap t)
{
    Object result;
    result.type = Object::Tilemap;
    result.tilemap = t;
    return result;
}

Object Scene::object(ObjEntity e)
{
    Object result;
    result.type = Object::Entity;
    result.entity = e;
    return result;
}

Object Scene::object(ObjBackdrop b)
{
    Object result;
    result.type = Object::Backdrop;
    result.backdrop = b;
    return result;
}

void Scene::update(ResourceSpec &resources)
{
    PhysicsManifolds manifolds = physics.detect_collisions();
    physics.resolve_physics(manifolds);
    manifolds.manifolds.deinit();

    for (auto [id, object] : iter(objects))
    {
        switch (object.type)
        {
        case Object::Tilemap:
            object.tilemap.update(physics, resources);
            break;
        case Object::Entity:
            object.entity.update(physics, resources);
            break;
        case Object::Backdrop:
            object.backdrop.update(physics, resources);
            break;
        }
    }
}

void Scene::render(BoxdrawRenderer &renderer, ResourceSpec &resources,
                   bool show_physics)
{
    for (auto [id, object] : iter(objects))
    {
        switch (object.type)
        {
        case Object::Tilemap:
            object.tilemap.render(renderer, resources);
            break;
        case Object::Entity:
            object.entity.render(renderer, resources);
            break;
        case Object::Backdrop:
            object.backdrop.render(renderer, resources);
            break;
        }
    }

    if (show_physics)
    {
        for (auto [id, body] : iter(physics.bodies))
        {
            Vector4 color = {1, 0, 0, 1};

            if (body.solid)
            {
                color.y = 1;
            }

            Texture tex = resources.tileset.cropped({64, 192, 32, 32});
            Box3 box = {};
            box.min = {body.area.pos.x - body.area.siz.x / 2, 0 - 0.5f,
                       body.area.pos.y - body.area.siz.y / 2};
            box.max = {body.area.pos.x + body.area.siz.x / 2, 1 - 0.5f,
                       body.area.pos.y + body.area.siz.y / 2};

            boxdraw_push(&renderer, boxdraw_cmdtexture(box, tex, true));
        }
    }
}

void Scene::deinit()
{
    for (auto [id, object] : iter(objects))
    {
        switch (object.type)
        {
        case Object::Tilemap:
            object.tilemap.deinit();
            break;
        case Object::Entity:
            object.entity.deinit();
            break;
        case Object::Backdrop:
            object.backdrop.deinit();
            break;
        }
    }

    this->objects.deinit();
}
