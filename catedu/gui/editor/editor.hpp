#pragma once
#include "../debugtree/debugtree.hpp"
#include "catedu/rendering/3d/boxdraw.hpp"
#include "catedu/rendering/3d/camera.hpp"
#include "catedu/rendering/resource_spec.hpp"
#include "catedu/scene/scene.hpp"
#include "catedu/sys/input.hpp"
#include "catedu/ui/rendering/core.hpp"
#include "catedu/ui/rendering/font.hpp"
#include "catedu/ui/user.hpp"
#include <sokol/sokol_app.h>

struct EditAction
{
    enum Type
    {
        None,
        CreateEntity,
        DeleteEntity,
        MoveEntity,
        PlaceTile,
        Count_,
    } type;

    union Cmd {
        struct CreateEntity
        {
            TableId entity_id;
            Object entity;
        } create_entity;
        struct DeleteEntity
        {
            TableId entity_id;
            Object entity_data;
        } delete_entity;
        struct MoveEntity
        {
            TableId entity;
            Vector2 current_pos;
            Vector2 pos;
        } move_entity;
        struct PlaceTile
        {
            TableId tilemap_entity;
            Vector2i pos;
            int prev_id; // 0 is empty
            int tile_id; // 0 is empty
        } place_tile;
    } cmd;
};

struct GuiEditor
{
    bool playtesting;
    bool create_entity;
    bool placing_object;
    bool dirty;
    bool exit_requested;
    UiState *ui_state;
    TableId selection;
    TableId tile_selection;
    size_t entity_list_page;
    Camera camera;
    GuiDebugTree debug_tree;
    Vector2 object_cursor_at;
    EditAction actions[512];
    size_t action_count;
    Scene *playtest_scene;

    static GuiEditor init(UiState *ui_state);
    bool show(BoxdrawRenderer &renderer, ResourceSpec &resources, Scene &scene,
              Input &input, UiUser **user, void *umka, bool *reload_module);
    void deinit();
};
