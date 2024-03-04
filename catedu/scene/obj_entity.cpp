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

        body.area = {(float)this->pos.x + model.model.min.x,
                     (float)this->pos.y + model.model.min.z,
                     model.model.max.x - model.model.min.x,
                     model.model.max.z - model.model.min.z};

        if (strcmp(model.name, "player") == 0 ||
            strcmp(model.name, "entity") == 0)
        {
            body.solid = true;
            body.dynamic = true;
        }

        body_id = world.bodies.allocate(body);
    }
    else
    {
        PhysicsBody &body = world.bodies.get_assert(body_id);
        this->pos = body.area.pos;
    }
}

void ObjEntity::render(BoxdrawRenderer &renderer, ResourceSpec &resources)
{
    Vector3 pos = {this->pos.x, 0, this->pos.y};

    TableId model = resources.find_model_by_name(model_name);
    if (model.id == 0)
    {
        model = resources.find_model_by_name("invalid");
    }

    render_model_at(pos, resources, model, renderer, true);
}
