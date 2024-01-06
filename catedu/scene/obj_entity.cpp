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

void ObjEntity::render(BoxdrawRenderer &renderer, ResourceSpec &resources)
{
    Vector3 pos = {this->pos.x, 0, this->pos.y};

    TableId model = resources.find_model_by_name(model_name);
    if (model.id == 0)
    {
        model = resources.find_model_by_name("invalid");
    }

    render_model_at(pos, resources, model, renderer, false);
}
