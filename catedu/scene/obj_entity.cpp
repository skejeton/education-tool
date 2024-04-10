#include "obj_entity.hpp"
#include "catedu/rendering/render_model.hpp"

ObjEntity ObjEntity::init(const char *model_name, Vector2 pos)
{
    ObjEntity result = {};
    result.pos = pos;

    // FIXME: This is a hack.
    strcpy(result.model_name, model_name);
    return result;
}

void ObjEntity::deinit()
{
}

void ObjEntity::update(PhysicsWorld &world, ResourceSpec &resources)
{
    if (body_id.id == 0)
    {
        PhysicsBody body = {0};
        TableId model_id = resources.find_model_by_name(this->model_name);
        if (model_id.id == 0)
        {
            model_id = resources.find_model_by_name("invalid");
        }
        SpecModel model = resources.models.get_assert(model_id);

        body.area = {(float)this->pos.x, (float)this->pos.y, 1, 1};

        body.solid = true;
        body.dynamic = true;

        body_id = world.bodies.allocate(body);
    }
    else
    {
        PhysicsBody &body = world.bodies.get_assert(body_id);
        this->pos = body.area.pos;
        this->pos.x += body.area.siz.x / 2;
        this->pos.y += body.area.siz.y / 2;
    }
}

void ObjEntity::render(catedu::pbr::Renderer &renderer, ResourceSpec &resources)
{
    Vector3 pos = {this->pos.x, 0, this->pos.y};

    TableId model = resources.find_model_by_name(model_name);
    if (model.id == 0)
    {
        model = resources.find_model_by_name("invalid");
    }

    render_model_at(pos, resources, model, renderer, true);
}

ObjEntity ObjEntity::copy()
{
    ObjEntity result = *this;
    result.body_id = {0};
    return result;
}

ObjEntity ObjEntity::load(void **data_)
{
    uint8_t *data = (uint8_t *)*data_;

    ObjEntity result = {};
    memcpy(result.model_name, data, 32);
    data += 32;
    memcpy(&result.pos, data, sizeof(Vector2));
    data += sizeof(Vector2);

    uint32_t count;
    memcpy(&count, data, sizeof(uint32_t));
    data += sizeof(uint32_t);

    for (uint32_t i = 0; i < count; i++)
    {
        Dialog dialog;
        memcpy(&dialog, data, sizeof(Dialog));
        data += sizeof(Dialog);
        result.dialog.dialogs.push(dialog);
    }

    *data_ = data;

    return result;
}

void ObjEntity::save(BumpAllocator &alloc)
{
    memcpy(alloc.alloc(32), this->model_name, 32);
    memcpy(alloc.alloc(sizeof(Vector2)), &this->pos, sizeof(Vector2));
    memcpy(alloc.alloc(sizeof(uint32_t)), &this->dialog.dialogs.count,
           sizeof(uint32_t));
    for (auto [id, dialog] : iter(this->dialog.dialogs))
    {
        memcpy(alloc.alloc(sizeof(Dialog)), &dialog, sizeof(Dialog));
    }
}
