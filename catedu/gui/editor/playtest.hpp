#pragma once
#include "catedu/scene/scene.hpp"

enum class [[nodiscard]] PlaytestCreateError
{
    None,
    NoPlayer,
};

struct Playtest
{
    TableId player;
    Camera camera;
    Scene scene;
    DialogList *dialog;
    int conversation_stage;

    static PlaytestCreateError from_scene_copy(Playtest &out_, Scene &scene,
                                               Camera camera);
    void deinit();

    void handle_update(Input &input, ResourceSpec &resources, void *umka);
    void handle_gui(UiUser &user, ResourceSpec &resources, bool *reload_module,
                    void *umka);
    void handle_render(catedu::pbr::Renderer &renderer, ResourceSpec &resources,
                       bool show_debug);
};
