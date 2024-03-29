#include "scene.hpp"
#include "catedu/core/alloc/bump_allocator.hpp"

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

Object Scene::prune_object(ObjectId id)
{
    Object obj = objects.get_assert(id);
    objects.remove(id);
    return obj;
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
    Object result = {};
    result.type = Object::Tilemap;
    result.tilemap = t;
    return result;
}

Object Scene::object(ObjEntity e)
{
    Object result = {};
    result.type = Object::Entity;
    result.entity = e;
    return result;
}

Object Scene::object(ObjBackdrop b)
{
    Object result = {};
    result.type = Object::Backdrop;
    result.backdrop = b;
    return result;
}

Scene Scene::copy()
{
    Scene result = {};

    for (auto [id, object] : iter(objects))
    {
        result.add_object(object.copy());
    }

    return result;
}

Buffer Scene::save()
{
    Buffer buf = {};

    BumpAllocator alloc = BumpAllocator::init();
    buf.data = alloc.memory;
    *(uint32_t *)alloc.alloc(sizeof(uint32_t)) = objects.count;
    for (auto [id, object] : iter(objects))
    {
        *(uint32_t *)alloc.alloc(sizeof(uint32_t)) = object.type;
        *(uint32_t *)alloc.alloc(sizeof(uint32_t)) = object.hide;
        memcpy(alloc.alloc(32), object.id, 32);
        memcpy(alloc.alloc(32), object.name, 32);
        switch (object.type)
        {
        case Object::Tilemap:
            object.tilemap.save(alloc);
            break;
        case Object::Entity:
            object.entity.save(alloc);
            break;
        case Object::Backdrop:
            object.backdrop.save(alloc);
            break;
        }
    }
    buf.size = alloc.offset;

    return buf;
}

Scene Scene::load(Buffer buffer)
{
    Scene result = {};

    uint8_t *data = (uint8_t *)buffer.data;
    uint32_t count = *(uint32_t *)buffer.data;

    data += sizeof(uint32_t);

    for (uint32_t i = 0; i < count; i++)
    {
        Object pobj = {}, obj;
        pobj.type = (Object::Type)(*(uint32_t *)data);
        data += sizeof(uint32_t);
        pobj.hide = *(uint32_t *)data;
        data += sizeof(uint32_t);
        memcpy(pobj.id, data, 32);
        data += 32;
        memcpy(pobj.name, data, 32);
        data += 32;
        switch (pobj.type)
        {
        case Object::Tilemap:
            obj = object(ObjTilemap::load((void **)&data));
            break;
        case Object::Entity:
            obj = object(ObjEntity::load((void **)&data));
            break;
        case Object::Backdrop:
            obj = object(ObjBackdrop::load((void **)&data));
            break;
        }
        obj.hide = pobj.hide;
        memcpy(obj.id, pobj.id, 32);
        memcpy(obj.name, pobj.name, 32);
        result.add_object(obj);
    }

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

void Scene::render(catedu::pbr::Renderer &renderer, ResourceSpec &resources,
                   bool show_physics)
{
    for (auto [id, object] : iter(objects))
    {
        if (object.hide)
        {
            continue;
        }

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
        /*
        assert("Not implemented" && false);
        for (auto [id, body] : iter(physics.bodies))
        {
            Vector4 color = {1, 0, 0, 1};

            if (body.solid)
            {
                color.y = 1;
            }

            Texture tex = resources.tileset.cropped({64, 192, 32, 32});
            Box3 box = {};
            box.min = {body.area.pos.x, 0 - 0.5f, body.area.pos.y};
            box.max = {body.area.pos.x + body.area.siz.x, 1 - 0.5f,
                       body.area.pos.y + body.area.siz.y};

            boxdraw_push(&renderer, boxdraw_cmdtexture(box, tex, true));
        }
        */
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
    this->physics.bodies.deinit();
}

Object Object::copy()
{
    Object result = {};
    result.type = type;

    switch (type)
    {
    case Object::Tilemap:
        result.tilemap = tilemap.copy();
        break;
    case Object::Entity:
        result.entity = entity.copy();
        break;
    case Object::Backdrop:
        result.backdrop = backdrop.copy();
        break;
    }

    memcpy(result.id, id, 32);
    memcpy(result.name, name, 32);
    result.hide = hide;

    return result;
}
