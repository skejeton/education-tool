#include "playtest.hpp"
#include "catedu/ui/widgets.hpp"
#include "umka_api.h"

ObjectId find_object_within_distance_not_player(Scene &scene, Vector2 pos,
                                                float distance)
{
    for (auto [id, obj] : iter(scene.objects))
    {
        if (obj.type == Object::Type::Entity)
        {
            Vector2 obj_pos = obj.entity.pos;
            if (vector2_cmp_distance(pos, obj_pos) < distance &&
                strcmp(obj.id, "player") != 0)
            {
                return id;
            }
        }
    }
    return NULL_ID;
}

PlaytestCreateError Playtest::from_scene_copy(Playtest &out_, Scene &scene,
                                              Camera camera)
{
    ObjectId player = scene.find_object("player");
    if (player == NULL_ID || scene.get_object(player)->type != Object::Entity)
    {
        return PlaytestCreateError::NoPlayer;
    }

    Playtest playtest = {};
    playtest.scene = scene.copy();
    playtest.player = playtest.scene.find_object("player");
    playtest.camera = camera;
    out_ = playtest;

    return PlaytestCreateError::None;
}

void Playtest::deinit()
{
    scene.deinit();
}

void Playtest::handle_update(Input &input, ResourceSpec &resources, void *umka)
{
    Object *obj = this->scene.get_object(this->player);

    camera.position = Vector3{obj->entity.pos.x, 10, obj->entity.pos.y - 10};
    camera.move(0, 0, 0);

    PhysicsBody *body = scene.physics.bodies.get(obj->entity.body_id);

    if (input.k[SAPP_KEYCODE_A].held)
    {
        body->area.pos.x -= 0.1;
    }
    if (input.k[SAPP_KEYCODE_D].held)
    {
        body->area.pos.x += 0.1;
    }
    if (input.k[SAPP_KEYCODE_W].held)
    {
        body->area.pos.y += 0.1;
    }
    if (input.k[SAPP_KEYCODE_S].held)
    {
        body->area.pos.y -= 0.1;
    }
    if (input.k[SAPP_KEYCODE_SPACE].pressed)
    {

        const char *source = "";

        ObjectId coll_id =
            find_object_within_distance_not_player(scene, obj->entity.pos, 3);

        DialogList *dialog = nullptr;
        if (coll_id != NULL_ID)
        {
            Object *coll_obj = scene.get_object(coll_id);
            if (coll_obj->type == Object::Type::Entity)
            {
                dialog = &coll_obj->entity.dialog;
                source = coll_obj->id;
            }
        }

        int func = umkaGetFunc(umka, NULL, "onInteract");
        assert(func);
        UmkaStackSlot id;

        id.ptrVal = umkaMakeStr(umka, (char *)source);

        if (umkaGetError(umka)->code == 0)
        {
            umkaCall(umka, func, 1, &id, NULL);
        }

        if (dialog)
        {
            this->dialog = dialog;
            this->conversation_stage = 0;
        }
    }

    this->scene.update(resources);
}

void Playtest::handle_gui(UiUser &user, ResourceSpec &resources,
                          bool *reload_module, void *umka)
{
    if (this->dialog)
    {
        int i = 0;
        for (auto dialog : iter(this->dialog->dialogs))
        {
            if (conversation_stage == i)
            {
                const char *options[] = {"Next", NULL};
                switch (msgbox(user, dialog.name, dialog.text, MsgBoxType::Info,
                               options))
                {
                case 0:
                    this->conversation_stage++;
                    goto end;
                }
            }
            i++;
        }
    end:;
    }

    int func = umkaGetFunc(umka, NULL, "onUI");

    if (UmkaError *error = umkaGetError(umka); error->code != 0)
    {
        const char *buttons[] = {"Retry", NULL};

        switch (msgbox(user, "Umka Error",
                       stdstrfmt("%s\n    at %s:%d", error->msg,
                                 error->fileName, error->line)
                           .c_str(),
                       MsgBoxType::Error, buttons))
        {
        case 0:
            *reload_module = true;
            break;
        }
    }
    else
    {
        UmkaStackSlot empty;
        umkaCall(umka, func, 0, NULL, &empty);
    }
}

void Playtest::handle_render(catedu::pbr::Renderer &renderer,
                             ResourceSpec &resources, bool show_debug)
{
    scene.render(renderer, resources, show_debug);
}
